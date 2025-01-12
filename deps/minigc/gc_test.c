#include "gc.c"
#include <stdio.h>


int
main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "test") == 0)  test();
    return 0;
}
