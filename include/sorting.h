
#ifndef SORTING_H
#define SORTING_H

#include <sys/stat.h>
#include "options.h"

typedef struct {
    char *name;
    char *path;
    struct stat info;
} FileEntry;

void sort_entries(FileEntry *entries, size_t count,
                  const Options *opts);

#endif
