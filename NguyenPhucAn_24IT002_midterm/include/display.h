
#ifndef DISPLAY_H
#define DISPLAY_H

#include "sorting.h"

void display_entry(const FileEntry *entry,
                   const Options *opts);

void display_total(const FileEntry *entries, size_t count,
                   const Options *opts);

#endif
