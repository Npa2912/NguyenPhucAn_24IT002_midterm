#ifndef LISTING_H
#define LISTING_H
#include "options.h"
int list_path(const char *path, const Options *opts);
int list_operands(int count, char **paths, const Options *opts);
#endif
