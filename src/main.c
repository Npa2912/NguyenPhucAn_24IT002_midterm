
#include <stdio.h>

#include "options.h"
#include "listing.h"

/*
 * Hàm main:
 * - Phân tích tùy chọn dòng lệnh.
 * - Xử lý đường dẫn mặc định.
 * - Hỗ trợ nhiều đường dẫn.
 * - Trả về mã lỗi nếu có lỗi xảy ra.
 */
int main(int argc, char *argv[])
{
    Options opts;

    int first_path;
    int status = 0;
    int i;

    /*
     * Phân tích các tùy chọn.
     */
    if (parse_options(
            argc,
            argv,
            &opts,
            &first_path
        ) != 0) {

        return 1;
    }

    /*
     * Không có đường dẫn:
     * Liệt kê thư mục hiện tại.
     */
    if (first_path == argc) {
        return list_path(".", &opts);
    }

    /*
     * Xử lý các đường dẫn được truyền vào.
     */
    for (i = first_path; i < argc; i++) {

        /*
         * In tên đường dẫn nếu có nhiều toán hạng.
         */
        if (argc - first_path > 1 &&
            !opts.directory_only) {

            printf("%s:\n", argv[i]);
        }

        if (list_path(argv[i], &opts) != 0) {
            status = 1;
        }

        /*
         * Ngăn cách kết quả của các đường dẫn.
         */
        if (i + 1 < argc) {
            printf("\n");
        }
    }

    return status;
}
