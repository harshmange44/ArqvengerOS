#include "shell/wd/wd.h"
#include "library/stdio.h"
#include "constants.h"
#include "library/stdlib.h"
#include "library/string.h"
#include "mcurses/mcurses.h"
#include "system/stat.h"
#include "system/fs/types.h"
#include "system/accessControlList/users.h"
#include "system/accessControlList/groups.h"
#include "system/call/ioctl/keyboard.h"

static void print_flag(int value, int flag, char c);

void command_cd(char* argv) {

    char* cmdEnd = strchr(argv, ' ');
    if (cmdEnd == NULL) {
        printf("Usage: cd path\n");
        return;
    }

    cmdEnd++;
    if (chdir(cmdEnd) == -1) {
        //printf("%s is not a directory.\n", cmdEnd);
        printf("Operation Unseccesful.\n");
    }
}

void man_cd(void) {
    setBold(1);
    printf("Usage:\n\t cd");
    setBold(0);

    printf(" DIRECTORY.\n");
}

void command_pwd(char* argv) {
    char cwd[512];
    if (getcwd(cwd, 512) == 0) {
        printf("%s\n", cwd);
    } else {
        printf("Oops. Something went horribly wrong! Please do tell Champo about it.");
    }
}

void man_pwd(void) {
    setBold(1);
    printf("Usage:\n\t pwd\n");
    setBold(0);
}

void command_mkdir(char* argv) {

    char* cmdEnd = strchr(argv, ' ');
    if (cmdEnd == NULL) {
        printf("Usage: mkdir path\n");
        return;
    }

    cmdEnd++;
    switch (mkdir(cmdEnd, 0744)) {
        case -1:
            printf("Cant create '%s'.\n"/*: No such file or directory*/, cmdEnd);
            break;
        case EEXIST:
            printf("Cant create '%s': File already exists.\n", cmdEnd);
            break;
        case EIO:
            printf("Cant create '%s': Some wizardry went awry. Try again later.\n", cmdEnd);
            break;
    }
}

void manMkdir(void) {
    setBold(1);
    printf("Usage:\n\tmkdir ");
    setBold(0);
    printf("DIRECTORY.\n");
}

void command_rmdir(char* argv) {

    char* cmdEnd = strchr(argv, ' ');
    if (cmdEnd == NULL) {
        printf("Usage: rmdir path\n");
        return;
    }

    cmdEnd++;
    switch (rmdir(cmdEnd)) {
        case -1:
        case ENOENT:
            printf("Cant remove '%s'.\n"/*: No such file or directory.\n"*/, cmdEnd);
            break;
        case EIO:
            printf("Cant remove '%s': Some wizardry went awry. Try again later.\n", cmdEnd);
            break;
        case ENOTEMPTY:
            printf("Cant remove '%s': The directory is not empty.\n", cmdEnd);
            break;
    }
}

void manRmdir(void) {
    setBold(1);
    printf("Usage:\n\trmdir ");
    setBold(0);
    printf("DIRECTORY.\n");
}

void print_flag(int value, int flag, char c) {
    if (value & flag) {
        putchar(c);
    } else {
        putchar('-');
    }
}

void command_ls(char* argv) {

    int fd;
    int hidden = 0;
    char* cmdEnd = strchr(argv, ' ');

    if (strlen(cmdEnd + 1) >= 2) {
        if (strncmp(cmdEnd + 1, "-a", 2) == 0) {
            hidden = 1;
            cmdEnd = strchr(cmdEnd + 1, ' ');
        }
    }

    if (cmdEnd != NULL) {
        chdir(cmdEnd + 1);
    }

    fd = open(".", O_RDONLY);
    if (fd == -1) {
        printf("The directory specified could not be opened\n");
        return;
    }

    termios inputStatus;
    termios lsStatus = { 0, 0 };
    ioctl(0, TCGETS, (void*) &inputStatus);
    ioctl(0, TCSETS, (void*) &lsStatus);

    size_t usableLines = TOTAL_ROWS - 1, i = 0;
    struct fs_DirectoryEntry entry;
    while (readdir(fd, &entry, hidden) == 1) {

        struct stat data;
        if (stat(entry.name, &data) == -1) {
            printf("Opps. Something went awry, try again. Or maybe tell your sysadmin about.");
            ioctl(0, TCSETS, (void*) &inputStatus);
            exit();
        }

        if (i >= usableLines) {
            printf(" -- Press enter to see more (or q to quit) -- ");
            int c;
            while ((c = getchar()) != '\n') {
                if (c == 'q') {
                    printf("\n");
                    ioctl(0, TCSETS, (void*) &inputStatus);
                    return;
                }
            }

            moveCursorInRow(1);
            clearLine(ERASE_ALL);
        }
        i++;

        struct User* user = get_user_by_id(data.uid);
        struct Group* group = get_group_by_id(data.gid);

        switch (data.type) {
            case INODE_FILE:
                putchar('-');
                break;
            case INODE_LINK:
                putchar('l');
                break;
            case INODE_DIR:
                putchar('d');
                break;
            case INODE_FIFO:
                putchar('p');
                break;
            case INODE_CHARDEV:
                putchar('c');
                break;
        }
        print_flag(data.mode, S_IRUSR, 'r');
        print_flag(data.mode, S_IWUSR, 'w');
        print_flag(data.mode, S_IXUSR, 'x');
        print_flag(data.mode, S_IRGRP, 'r');
        print_flag(data.mode, S_IWGRP, 'w');
        print_flag(data.mode, S_IXGRP, 'x');
        print_flag(data.mode, S_IROTH, 'r');
        print_flag(data.mode, S_IWOTH, 'w');
        print_flag(data.mode, S_IXOTH, 'x');
        putchar(' ');

        if (user != NULL) {
            printf("%10s ", user->name);
        } else {
            printf("%10c ", ' ');
        }

        if (user != NULL) {
            printf("%10s ", group->name);
        } else {
            printf("%10c ", ' ');
        }

        printf("%7d %s\n", data.size, entry.name);
    }

    ioctl(0, TCSETS, (void*) &inputStatus);
}

void command_ln(char* argv) {
    char* target = strchr(argv, ' ');

    if (target == NULL) {
        printf("Arguments invalid.\n");
        return;
    }

    char* link = strchr(target + 1, ' ');

    if (link == NULL) {
        printf("Arguments invalid.\n");
        return;
    }

    int i;

    for (i = 1; target[i] != ' '; i++){

    }

    target[i] = '\0';

    i = symlink(link + 1, target + 1);

    if (i != 0) {
        printf("Operation unsuccesful.\n");
    }
}

void man_ln(void) {
    setBold(1);
    printf("Usage:\n\t ln");
    setBold(0);

    printf(" target link_name\n");

}

void manLs(void) {
    setBold(1);
    printf("Usage:\n\tls ");
    setBold(0);
    printf("[-a] [DIRECTORY].\n");
    printf("The -a options indicates that hidden files should be shown.\n");
    printf("The default DIRECTORY is the cwd.\n");
}

void command_chmod(char* argv) {
    char* stringmode = strchr(argv, ' ');

    if (stringmode == NULL) {
        printf("Arguments invalid.\n");
    }

    char* file = strchr(stringmode + 1, ' ');

    if (file == NULL) {
        printf("Arguments invalid.\n");
        return;
    }

    int i;

    for (i = 1; stringmode[i] != ' '; i++){

    }

    stringmode[i] = '\0';

    int mode = parseoct(stringmode + 1);

    if (mode == -1) {
        printf("The mode is not a valid number\n");
        return;
    }

    i = chmod(mode, file + 1);

    if (i != 0) {
        printf("Operation unsuccesful.\n");
    }
}

void man_chmod(void) {
    setBold(1);
    printf("Usage:\n\tchmod ");
    setBold(0);
    printf("MODE FILE.\n");
    printf("MODE is an octal value.\n");
}

void command_chown(char* argv) {

    char* cmdEnd = strchr(argv, ' ');
    if (cmdEnd == NULL) {
        printf("Usage: cd path\n");
        return;
    }

    cmdEnd++;
    if (chown(cmdEnd) != 0) {
        printf("Operation Unseccesful.\n");
    }
}

void man_chown(void) {
    setBold(1);
    printf("Usage:\n\t chown");
    setBold(0);
    printf(" FILE.\n");
}
