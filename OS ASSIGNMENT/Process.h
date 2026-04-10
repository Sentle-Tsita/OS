#include <stdio.h>

#ifndef PCB_H
#define PCB_H

typedef enum {
    READY, RUNNING
} ProcessState;

typedef struct PCB{
    unsigned int pid;
    unsigned int burstTime;
    unsigned int arrivalTime;
    unsigned int remainingTime;
    ProcessState state;
} PCB;

#endif
