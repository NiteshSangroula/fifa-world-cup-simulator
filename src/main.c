#include "test.h"

#include <stdio.h>
#include <stdlib.h>

#define SIMULATOR_VERSION "0.1.0"

int main(void)
{
    printf("========================================\n");
    printf("   FIFA World Cup Simulator v%s\n", SIMULATOR_VERSION);
    printf("========================================\n\n");

    test();

    printf("Tournament simulation coming soon...\n");

    return EXIT_SUCCESS;
}
