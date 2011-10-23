#include "system/call.h"
#include "drivers/keyboard.h"
#include "drivers/tty/tty.h"
#include "system/process/table.h"
#include "system/process/process.h"
#include "system/scheduler.h"
#include "system/fs/fs.h"
#include "system/fs/inode.h"
#include "system/fs/direntry.h"
#include "library/string.h"
#include "constants.h"

size_t _read(int fd, void* buf, size_t length) {

    struct Process* process = scheduler_current();
    struct FileDescriptor* fileDescriptor = &(process->fdTable[fd]);

    if (fileDescriptor->inode == NULL || fileDescriptor->ops->read == NULL) {
        return -1;
    }

    if ((fileDescriptor->flags & 3) == O_WRONLY) {
        return -1;
    }

    return fileDescriptor->ops->read(fileDescriptor, buf, length);
}

size_t _write(int fd, const void* buf, size_t length) {

    struct Process* process = scheduler_current();
    struct FileDescriptor* fileDescriptor = &(process->fdTable[fd]);

    if (fileDescriptor->inode == NULL || fileDescriptor->ops->write == NULL) {
        return -1;
    }

    if ((fileDescriptor->flags & 3) == O_RDONLY) {
        return -1;
    }

    return fileDescriptor->ops->write(fileDescriptor, buf, length);
}

int _open(const char* path, int flags, int mode) {
    struct fs_Inode* curdir;
    struct fs_DirectoryEntry nextdir;
    int index = 0;
    char entry[ENTRY_NAME_MAX_LEN];
    int len = strlen(path);
    int gid, uid, file_gid, file_uid;
    int fd = -1;
    int i = 0;

    struct Process* process = scheduler_current();

    for (i = 0; i < MAX_OPEN_FILES; i++) {
        if (process->fdTable[i].inode == NULL) {
            fd = i;
            break;
        }
    }

    if (fd == -1) {
        return -1;
    }

    if (path[index] == '/') {
        curdir = fs_root();
        index++;
    } else {
        //TODO curdir = getcwd();
    }

    for (int i = 0; index < len; index++) {

        entry[i] = path[index];
        i++;

        if (path[index] == '/' || index + 1 == len) {
            if (index + 1 != len) {
                i--;
            }

            entry[i] = '\0';
            i = 0;

            nextdir = fs_findentry(curdir, entry);

            if (nextdir.inode == 0) {
                if (index + 1 != len) {
                    fs_inode_close(curdir);
                    return -1;
                }
                if (!(flags & O_CREAT) || _creat(path, mode) != 0) {
                    fs_inode_close(curdir);
                    return -1;
                }
                nextdir = fs_findentry(curdir, entry);
            }
            fs_inode_close(curdir);
            curdir = fs_inode_open(nextdir.inode);
        }

    }

    file_uid = curdir->data->uid;
    file_gid = curdir->data->gid;
    gid = process->gid;
    uid = process->uid;

    unsigned short perms = fs_permission(curdir);

    if ((flags & 3) == O_RDONLY || (flags & 3) == O_RDWR) {
       if (!(perms & S_IROTH) &&
            !(perms & S_IRGRP && file_gid == gid) &&
            !(perms & S_IRUSR && file_uid == uid)) {
            fs_inode_close(curdir);
            return -1;
        }
    }

    if ((flags & 3) == O_WRONLY || (flags & 3) == O_RDWR) {
       if(!(perms & S_IWOTH) &&
          !(perms & S_IWGRP && file_gid == gid) &&
          !(perms & S_IWUSR && file_uid == uid)) {
            fs_inode_close(curdir);
            return -1;
        }
    }

    //Pasamos los permisos
    process->fdTable[fd] = fs_fd(curdir, flags);
    fs_inode_close(curdir);
    return fd;
}

int _creat(const char* path, int mode) {
    int index = 0;
    int len = strlen(path);
    int i = 0;
    char entry[ENTRY_NAME_MAX_LEN];
    struct fs_Inode* curdir;
    struct fs_DirectoryEntry nextdir;

    if (path[index] == '/') {
        curdir = fs_root();
        index++;
    } else {
        //TODO curdir = getcwd();
    }

    for (int i = 0; index < len; index++) {

        entry[i] = path[index];
        i++;

        if (index + 1 == len) {
            break;
        }

        if (path[index] == '/') {
            i--;
            entry[i] = '\0';
            i = 0;

            nextdir = fs_findentry(curdir, entry);

            if (nextdir.inode == 0) {
                return -1;
            }
            fs_inode_close(curdir);
            curdir = fs_inode_open(nextdir.inode);
        }

    }

    entry[i] = '\0';
    if (fs_mknod(curdir, entry, INODE_FILE) != 0) {
        return -1;
    }
    nextdir = fs_findentry(curdir, entry);
    if (nextdir.inode == 0) {
        return -1;
    }
    fs_inode_close(curdir);
    curdir = fs_inode_open(nextdir.inode);
    if (fs_set_permission(curdir, mode) != 0) {
        fs_inode_close(curdir);
        return -1;
    }
    fs_inode_close(curdir);
    return 0;

}

int _close(int fd) {

    struct Process* process = scheduler_current();
    struct FileDescriptor* fileDescriptor = &(process->fdTable[fd]);

    if (fileDescriptor->inode == NULL) {
        return -1;
    }
    if (fileDescriptor->ops->close != NULL) {
        fileDescriptor->ops->close(fileDescriptor);
    }
    fs_inode_close(fileDescriptor->inode);
    process->fdTable[fd].inode = NULL;
    return 0;

}

int _ioctl(int fd, int cmd, void* argp) {

    struct Process* process = scheduler_current();
    struct FileDescriptor* fileDescriptor = &(process->fdTable[fd]);

    if (fileDescriptor->inode == NULL || fileDescriptor->ops->ioctl == NULL) {
        return -1;
    }

    return fileDescriptor->ops->ioctl(fileDescriptor, cmd, argp);
}

int _mkdir(const char* path, int mode) {
}

int _rmdir(const char* path) {
}

int _unlink(const char* path) {
}

int _rename(const char* from, const char* to) {
}

int _readdir(int fd, struct fs_DirectoryEntry* entry) {
}

int _chdir(const char* path) {
}

int _getcwd(char* path, size_t len) {
}

