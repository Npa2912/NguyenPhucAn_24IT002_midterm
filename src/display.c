
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "display.h"
#include "utils.h"

#ifndef _WIN32
#include <unistd.h>
#endif

#ifdef _WIN32
#ifndef S_IXGRP
#define S_IXGRP 0010
#endif
#ifndef S_IXOTH
#define S_IXOTH 0001
#endif
#endif

static char file_suffix(mode_t mode)
{
    if (S_ISDIR(mode))
        return '/';

#ifdef S_ISLNK
    if (S_ISLNK(mode))
        return '@';
#endif

#ifdef S_ISFIFO
    if (S_ISFIFO(mode))
        return '|';
#endif

#ifdef S_ISSOCK
    if (S_ISSOCK(mode))
        return '=';
#endif

    if (mode & (S_IXUSR | S_IXGRP | S_IXOTH))
        return '*';

    return '\0';
}

static time_t selected_time(const FileEntry *entry,
                            const Options *opts)
{
    if (opts->change_time)
        return entry->info.st_ctime;

    if (opts->access_time)
        return entry->info.st_atime;

    return entry->info.st_mtime;
}

void display_entry(const FileEntry *entry,
                   const Options *opts)
{
    char permissions[11];
    char owner[128];
    char group[128];
    char size[64];
    char date[64];
    char suffix = '\0';

    if (opts->show_inode) {
        printf("%.0f ",
               (double)entry->info.st_ino);
    }

    if (opts->show_blocks) {
        long long blocks = file_blocks(
            &entry->info, opts->kilobytes);

        if (opts->human_readable) {
            char readable[64];

            format_size(
                blocks * (opts->kilobytes ? 1024 : 512),
                readable,
                sizeof(readable)
            );

            printf("%s ", readable);
        } else {
            printf("%.0f ", (double)blocks);
        }
    }

    if (opts->long_format) {
        format_permissions(
            entry->info.st_mode,
            permissions
        );

        format_owner(
            &entry->info,
            opts->numeric_ids,
            owner,
            sizeof(owner)
        );

        format_group(
            &entry->info,
            opts->numeric_ids,
            group,
            sizeof(group)
        );

        if (opts->human_readable) {
            format_size(
                (long long)entry->info.st_size,
                size,
                sizeof(size)
            );
        } else {
            snprintf(
                size,
                sizeof(size),
                "%.0f",
                (double)entry->info.st_size
            );
        }

        format_file_time(
            selected_time(entry, opts),
            date,
            sizeof(date)
        );

        printf(
            "%s %lu %-10s %-10s %8s %s ",
            permissions,
            (unsigned long)entry->info.st_nlink,
            owner,
            group,
            size,
            date
        );
    }

    if (opts->classify)
        suffix = file_suffix(entry->info.st_mode);

    printf("%s", entry->name);

    if (suffix != '\0')
        putchar(suffix);

#ifndef _WIN32
    if (opts->long_format &&
        S_ISLNK(entry->info.st_mode)) {

        char target[4096];

        ssize_t n = readlink(
            entry->path,
            target,
            sizeof(target) - 1
        );

        if (n >= 0) {
            target[n] = '\0';
            printf(" -> %s", target);
        }
    }
#endif

    putchar('\n');
}

void display_total(const FileEntry *entries,
                   size_t count,
                   const Options *opts)
{
    size_t i;
    long long total = 0;

    if (!opts->long_format && !opts->show_blocks)
        return;

    for (i = 0; i < count; i++) {
        total += file_blocks(
            &entries[i].info,
            opts->kilobytes
        );
    }

    if (opts->human_readable) {
        char readable[64];

        format_size(
            total * (opts->kilobytes ? 1024 : 512),
            readable,
            sizeof(readable)
        );

        printf("total %s\n", readable);
    } else {
        printf("total %.0f\n", (double)total);
    }
}
