#ifndef PROCESS_H
#define PROCESS_H

typedef enum {
    READY,
    RUNNING,
    FINISHED
} ProcessState;

typedef struct {
    int pid;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    ProcessState state;
} PCB;

#endif
