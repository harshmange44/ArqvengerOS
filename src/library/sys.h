#ifndef __LIBRARY_SYS__
#define __LIBRARY_SYS__

#include "type.h"
#include "system/pinfo.h"

void yield(void);

pid_t wait(void);

void exit(void);

pid_t run(void(*entryPoint)(char*), const char* args, int fg);

pid_t getpid(void);

pid_t getppid(void);

void kill(pid_t pid);

void sleep(int seconds);

int nice(int priority);

int renice(int pid, int priority);

size_t pinfo(struct ProcessInfo* data, size_t size);

void setProcessPersona(int pid, int uid, int gid);

void getProcessPersona(int pid, int* uid, int* gid);

#endif
