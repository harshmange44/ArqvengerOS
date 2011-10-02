#include "system/interrupt.h"
#include "shell/shell.h"
#include "library/stdio.h"
#include "library/string.h"
#include "drivers/tty/tty.h"
#include "drivers/keyboard.h"
#include "system/mm.h"
#include "system/common.h"
#include "system/gdt.h"
#include "system/process/table.h"
#include "drivers/ata.h"

void kmain(struct multiboot_info* info, unsigned int magic);

static void idle(char* unused) {
    while (1) {
        yield();
    }
}

/**
 * Kernel entry point
 */
void kmain(struct multiboot_info* info, unsigned int magic) {

    setupGDT();
    setupIDT();

    FILE files[3];
    for (int i = 0; i < 3; i++) {
        files[i].fd = i;
        files[i].flag = 0;
        files[i].unget = 0;
    }

    stdin = &files[0];
    stdout = &files[1];
    stderr = &files[2];

    initScreen();
    writeScreen("\033[1;1H\033[2J", 10);

    initKeyboard();
    initMemoryMap(info);
    ata_init(info);

    disableInterrupts();
    struct Process* idleProcess = process_table_new(idle, NULL, NULL);
    struct Process* shellProcess = process_table_new(shell, NULL, idleProcess);
    enableInterrupts();

    while (1) {}
}


