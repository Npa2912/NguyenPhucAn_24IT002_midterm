
#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <sys/stat.h>
#include <time.h>

void format_permissions(mode_t mode, char output[11]);
void format_size(long long bytes, char *output, size_t length);
void format_file_time(time_t value, char *output, size_t length);
void format_owner(const struct stat *st, int numeric,
                  char *output, size_t length);
void format_group(const struct stat *st, int numeric,
                  char *output, size_t length);
long long file_blocks(const struct stat *st, int kilobytes);

#endif
