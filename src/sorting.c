
#include <stdlib.h>
#include <string.h>
#include "sorting.h"

static const Options *current_opts;

static int compare_entries(const void *a, const void *b)
{
    const FileEntry *x = (const FileEntry *)a;
    const FileEntry *y = (const FileEntry *)b;
    int result = 0;

    if (current_opts->sort_size) {
        if (x->info.st_size < y->info.st_size)
            result = 1;
        else if (x->info.st_size > y->info.st_size)
            result = -1;
    } else if (current_opts->sort_time) {
        time_t tx = x->info.st_mtime;
        time_t ty = y->info.st_mtime;

        if (current_opts->change_time) {
            tx = x->info.st_ctime;
            ty = y->info.st_ctime;
        } else if (current_opts->access_time) {
            tx = x->info.st_atime;
            ty = y->info.st_atime;
        }

        if (tx < ty)
            result = 1;
        else if (tx > ty)
            result = -1;
    }

    if (result == 0)
        result = strcmp(x->name, y->name);

    return current_opts->reverse ? -result : result;
}

void sort_entries(FileEntry *entries, size_t count,
                  const Options *opts)
{
    if (opts->no_sort || count < 2)
        return;

    current_opts = opts;
    qsort(entries, count, sizeof(FileEntry), compare_entries);
}
