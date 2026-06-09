#include "lib.h"

int main(void)
{
    capp_begin("commandline.capp", "commandline.capp", "example command-line app compiled from C into a CAPP package");
    capp_println("commandline.capp: this executable was generated from commandline.c");
    capp_print("argv:");
    capp_print_args();
    capp_print("uptime ");
    capp_print_ticks();
    capp_exit(0);
    capp_end();
    return 0;
}
