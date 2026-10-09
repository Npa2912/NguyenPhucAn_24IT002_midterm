
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "utils.h"

#ifndef _WIN32
#include <pwd.h>
#include <grp.h>
#endif

#ifdef _WIN32
#ifndef S_IRGRP
#define S_IRGRP 0040
#define S_IWGRP 0020
#define S_IXGRP 0010
#define S_IROTH 0004
#define S_IWOTH 0002
#define S_IXOTH 0001
#endif
#endif

void format_permissions(mode_t mode, char output[11])
{
    strcpy(output, "----------");

    if (S_ISDIR(mode))
        output[0] = 'd';
#ifdef S_ISLNK
    else if (S_ISLNK(mode))
        output[0] = 'l';
#endif
#ifdef S_ISCHR
    else if (S_ISCHR(mode))
        output[0] = 'c';
#endif
#ifdef S_ISBLK
    else if (S_ISBLK(mode))
        output[0] = 'b';
#endif
#ifdef S_ISFIFO
    else if (S_ISFIFO(mode))
        output[0] = 'p';
#endif
#ifdef S_ISSOCK
    else if (S_ISSOCK(mode))
        output[0] = 's';
#endif

    if (mode & S_IRUSR) output[1] = 'r';
    if (mode & S_IWUSR) output[2] = 'w';
    if (mode & S_IXUSR) output[3] = 'x';

    if (mode & S_IRGRP) output[4] = 'r';
    if (mode & S_IWGRP) output[5] = 'w';
    if (mode & S_IXGRP) output[6] = 'x';

    if (mode & S_IROTH) output[7] = 'r';
    if (mode & S_IWOTH) output[8] = 'w';
    if (mode & S_IXOTH) output[9] = 'x';

#ifdef S_ISUID
    if (mode & S_ISUID)
        output[3] = (mode & S_IXUSR) ? 's' : 'S';
#endif

#ifdef S_ISGID
    if (mode & S_ISGID)
        output[6] = (mode & S_IXGRP) ? 's' : 'S';
#endif

#ifdef S_ISVTX
    if (mode & S_ISVTX)
        output[9] = (mode & S_IXOTH) ? 't' : 'T';
#endif
}

void format_size(long long bytes, char *output, size_t length)
{
    static const char *units[] = {
        "B", "K", "M", "G", "T", "P"
    };

    double size = (double)bytes;
    int unit = 0;

    while (size >= 1024.0 && unit < 5) {
        size /= 1024.0;
        unit++;
    }

    if (unit == 0)
        snprintf(output, length, "%.0fB", size);
    else if (size < 10.0)
        snprintf(output, length, "%.1f%s", size, units[unit]);
    else
        snprintf(output, length, "%.0f%s", size, units[unit]);
}

void format_file_time(time_t value, char *output, size_t length)
{
    struct tm *tm_info = localtime(&value);
    char month[16];
    char clock_text[16];

    if (tm_info == NULL) {
        snprintf(output, length, "?");
        return;
    }

    if (strftime(month, sizeof(month), "%b", tm_info) == 0 ||
        strftime(clock_text, sizeof(clock_text),
                 "%H:%M", tm_info) == 0) {
        snprintf(output, length, "?");
        return;
    }

    {
        time_t now = time(NULL);
        if (value > now + 3600 || value < now - (time_t)15552000) {
            char year[16];
            strftime(year, sizeof(year), "%Y", tm_info);
            snprintf(output, length, "%s %2d  %s", month, tm_info->tm_mday, year);
        } else {
            snprintf(output, length, "%s %2d %s", month, tm_info->tm_mday, clock_text);
        }
    }
}

void format_owner(const struct stat *st, int numeric,
                  char *output, size_t length)
{
#ifdef _WIN32
    (void)st;
    (void)numeric;
    snprintf(output, length, "-");
#else
    struct passwd *pw = numeric ? NULL : getpwuid(st->st_uid);

    if (pw != NULL)
        snprintf(output, length, "%s", pw->pw_name);
    else
        snprintf(output, length, "%lu",
                 (unsigned long)st->st_uid);
#endif
}

void format_group(const struct stat *st, int numeric,
                  char *output, size_t length)
{
#ifdef _WIN32
    (void)st;
    (void)numeric;
    snprintf(output, length, "-");
#else
    struct group *gr = numeric ? NULL : getgrgid(st->st_gid);

    if (gr != NULL)
        snprintf(output, length, "%s", gr->gr_name);
    else
        snprintf(output, length, "%lu",
                 (unsigned long)st->st_gid);
#endif
}

long long file_blocks(const struct stat *st, int kilobytes)
{
#ifdef _WIN32
    long long blocks = ((long long)st->st_size + 511) / 512;
#else
    long long blocks = (long long)st->st_blocks;
#endif

    return kilobytes ? (blocks + 1) / 2 : blocks;
}
