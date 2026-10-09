
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "listing.h"
#include "sorting.h"
#include "display.h"

/*
 * MinGW trên Windows không hỗ trợ lstat().
 * NetBSD vẫn dùng lstat() bình thường.
 */
#ifdef _WIN32
#define lstat stat
#endif

/*
 * Kiểm tra file có được hiển thị không.
 */
static int should_show(const char *name, const Options *opts)
{
    if (opts->show_all)
        return 1;

    if (strcmp(name, ".") == 0 ||
        strcmp(name, "..") == 0)
        return 0;

    if (opts->almost_all)
        return 1;

    return name[0] != '.';
}

/*
 * Sao chép chuỗi.
 */
static char *copy_string(const char *s)
{
    size_t n = strlen(s) + 1;
    char *p = malloc(n);

    if (p != NULL)
        memcpy(p, s, n);

    return p;
}

/*
 * Ghép đường dẫn.
 * Ví dụ: tests + folder1 -> tests/folder1
 */
static char *join_path(const char *dir, const char *name)
{
    size_t n = strlen(dir) + strlen(name) + 2;
    char *p = malloc(n);

    if (p != NULL)
        snprintf(p, n, "%s/%s", dir, name);

    return p;
}

/*
 * Giải phóng bộ nhớ của danh sách file.
 */
static void free_entries(FileEntry *entries, size_t count)
{
    size_t i;

    for (i = 0; i < count; i++) {
        free(entries[i].name);
        free(entries[i].path);
    }

    free(entries);
}

/*
 * Liệt kê nội dung thư mục.
 *
 * Quy trình:
 * 1. Mở thư mục.
 * 2. Đọc các file.
 * 3. Lấy thông tin bằng lstat().
 * 4. Sắp xếp.
 * 5. Hiển thị.
 * 6. Đệ quy nếu có -R.
 */
static int list_directory(const char *path,
                          const Options *opts,
                          int show_header)
{
    DIR *dir;
    struct dirent *item;

    FileEntry *entries = NULL;

    size_t count = 0;
    size_t capacity = 0;
    size_t i;

    int status = 0;

    dir = opendir(path);

    if (dir == NULL) {
        perror(path);
        return 1;
    }

    /*
     * Đọc toàn bộ mục trong thư mục.
     */
    while ((item = readdir(dir)) != NULL) {
        FileEntry entry;

        if (!should_show(item->d_name, opts))
            continue;

        entry.name = copy_string(item->d_name);
        entry.path = join_path(path, item->d_name);

        if (entry.name == NULL || entry.path == NULL) {
            free(entry.name);
            free(entry.path);

            fprintf(stderr, "myls: out of memory\n");
            status = 1;
            break;
        }

        /*
         * Lấy thông tin file.
         */
        if (lstat(entry.path, &entry.info) != 0) {
            perror(entry.path);

            free(entry.name);
            free(entry.path);

            status = 1;
            continue;
        }

        /*
         * Mở rộng mảng khi cần.
         */
        if (count == capacity) {
            size_t new_capacity =
                capacity ? capacity * 2 : 16;

            FileEntry *tmp;

            if (new_capacity > ((size_t)-1) /
                               sizeof(FileEntry)) {
                fprintf(stderr,
                        "myls: too many entries\n");

                free(entry.name);
                free(entry.path);

                status = 1;
                break;
            }

            tmp = realloc(
                entries,
                new_capacity * sizeof(FileEntry)
            );

            if (tmp == NULL) {
                free(entry.name);
                free(entry.path);

                fprintf(stderr,
                        "myls: out of memory\n");

                status = 1;
                break;
            }

            entries = tmp;
            capacity = new_capacity;
        }

        entries[count++] = entry;
    }

    if (closedir(dir) != 0) {
        perror("closedir");
        status = 1;
    }

    /*
     * Sắp xếp theo các tùy chọn:
     * -S, -t, -r, -f.
     */
    sort_entries(entries, count, opts);

    /*
     * Hiển thị tiêu đề thư mục.
     */
    if (show_header)
        printf("%s:\n", path);

    /*
     * Hiển thị tổng số block khi có -l hoặc -s.
     */
    display_total(entries, count, opts);

    /*
     * Hiển thị toàn bộ file trong thư mục hiện tại.
     */
    for (i = 0; i < count; i++) {
        display_entry(&entries[i], opts);
    }

    /*
     * Đệ quy sau khi đã hiển thị xong
     * nội dung thư mục hiện tại.
     */
    if (opts->recursive) {
        for (i = 0; i < count; i++) {

            /*
             * Tránh đệ quy vô hạn với "." và "..".
             */
            if (strcmp(entries[i].name, ".") == 0 ||
                strcmp(entries[i].name, "..") == 0) {
                continue;
            }

            if (S_ISDIR(entries[i].info.st_mode)) {
                printf("\n");

                if (list_directory(
                        entries[i].path,
                        opts,
                        1
                    ) != 0) {
                    status = 1;
                }
            }
        }
    }

    free_entries(entries, count);

    return status;
}

/*
 * Xử lý một đường dẫn:
 *
 * - File thông thường: hiển thị thông tin.
 * - Thư mục + -d: hiển thị như file.
 * - Thư mục bình thường: liệt kê nội dung.
 */
int list_path(const char *path, const Options *opts)
{
    struct stat info;

    if (lstat(path, &info) != 0) {
        perror(path);
        return 1;
    }

    if (!S_ISDIR(info.st_mode) ||
        opts->directory_only) {

        FileEntry entry;

        entry.name = (char *)path;
        entry.path = (char *)path;
        entry.info = info;

        display_entry(&entry, opts);

        return 0;
    }

    return list_directory(
        path,
        opts,
        opts->recursive
    );
}
