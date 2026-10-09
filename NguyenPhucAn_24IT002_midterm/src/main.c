#include "options.h"
#include "listing.h"
int main(int argc, char **argv)
{
    Options opts;
    int first;
    if (parse_options(argc, argv, &opts, &first) != 0) return 1;
    return list_operands(argc - first, argv + first, &opts);
}
