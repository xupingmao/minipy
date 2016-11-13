#include "vm.c"
#include "bin.c"

int main(int argc, char *argv[])
{
    /* start vm with bin */
    return tm_run(argc, argv, bin);
}
