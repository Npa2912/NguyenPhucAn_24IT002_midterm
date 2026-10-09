
#include <stdio.h>
#include <string.h>
#include "options.h"

int parse_options(int argc, char *argv[], Options *opts,
                  int *first_path)
{
    int i, j;

    memset(opts, 0, sizeof(*opts));

    for (i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "--") == 0) {
            i++;
            break;
        }

        if (arg[0] != '-' || arg[1] == '\0') {
            break;
        }

        for (j = 1; arg[j] != '\0'; j++) {
            switch (arg[j]) {
                case 'a':
                    opts->show_all = 1;
                    break;
                case 'A':
                    opts->almost_all = 1;
                    break;
                case 'c':
                    opts->change_time = 1;
                    opts->access_time = 0;
                    break;
                case 'd':
                    opts->directory_only = 1;
                    opts->recursive = 0;
                    break;
                case 'F':
                    opts->classify = 1;
                    break;
                case 'f':
                    opts->no_sort = 1;
                    break;
                case 'h':
                    opts->human_readable = 1;
                    opts->kilobytes = 0;
                    break;
                case 'i':
                    opts->show_inode = 1;
                    break;
                case 'k':
                    opts->kilobytes = 1;
                    opts->human_readable = 0;
                    break;
                case 'l':
                    opts->long_format = 1;
                    opts->numeric_ids = 0;
                    break;
                case 'n':
                    opts->numeric_ids = 1;
                    opts->long_format = 1;
                    break;
                case 'q':
                    opts->escape_names = 1;
                    opts->raw_names = 0;
                    break;
                case 'R':
                    opts->recursive = 1;
                    opts->directory_only = 0;
                    break;
                case 'r':
                    opts->reverse = 1;
                    break;
                case 'S':
                    opts->sort_size = 1;
                    opts->sort_time = 0;
                    break;
                case 's':
                    opts->show_blocks = 1;
                    break;
                case 't':
                    opts->sort_time = 1;
                    opts->sort_size = 0;
                    break;
                case 'u':
                    opts->access_time = 1;
                    opts->change_time = 0;
                    break;
                case 'w':
                    opts->raw_names = 1;
                    opts->escape_names = 0;
                    break;
                default:
                    fprintf(stderr,
                            "myls: illegal option -- %c\n",
                            arg[j]);
                    return -1;
            }
        }
    }

    *first_path = i;
    return 0;
}
