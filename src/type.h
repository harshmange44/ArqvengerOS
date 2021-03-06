#ifndef _type_header
#define _type_header

#include "library/stddef.h"

typedef unsigned char byte;
typedef short word;
typedef int dword;

typedef unsigned int time_t;

typedef int pid_t;

#define TICKS_SAMPLE_SIZE 10
#define MILLISECONDS_PER_TICK 55

#define MIN_PRIORITY 0
#define MAX_PRIORITY 3
#define INVALID_PRIORITY -1

#endif
