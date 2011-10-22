#include "shell/cat/cat.h"
#include "library/stdio.h"
#include "library/stdio.h"
#include "library/stdlib.h"
#include "library/string.h"
#include "constants.h"
#include "mcurses/mcurses.h"

void cat(char* argv) {

    char* cmdEnd = strchr(argv, ' ');
    if (cmdEnd == NULL) {
        printf("cat needs at least one parameter\n");
        return;
    }

    cmdEnd++;
    printf("Doing open... ");
    int fd = open(cmdEnd, O_RDWR);
    if (fd < 0) {
        printf("Cant open the file. Sorry dude.\n");
    }
    printf("Open done\n");

    char c;
    while (read(fd, &c, 1) == 1) {
        //putchar(c);
    }
    printf("\n");
}

void manCat(void) {
    setBold(1);
    printf("Usage:\n\t cat");
    setBold(0);

    printf(" file\n");
}

