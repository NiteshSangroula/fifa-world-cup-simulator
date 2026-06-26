#include "display.h"
#include "test_utils.h"

#include <stdio.h>

void test_display(void)
{
    printf("========== Display tests ==========\n");
    printMainMenu();
    printTournamentMenu(GROUP_STAGE);
    printCRUDMenu();
    printStatsMenu();

    // print_summary();
}
