#include <stdio.h>
#include <stdlib.h>
#include "process.h"

#define MAX_PROCESSES 100
#define TIME_QUANTUM 3   // Round Robin time slice

// ---------------- QUEUE ----------------

typedef struct {
    PCB data[MAX_PROCESSES];
    int front;
    int rear;
    int size;
} Queue;

void initQueue(Queue* q) {
    q->front = 0;
    q->rear = -1;
    q->size = 0;
}

int isEmpty(Queue* q) {
    return q->size == 0;
}

int enqueue(Queue* q, PCB p) {
    if (q->size == MAX_PROCESSES) {
        printf("Error: Ready queue full! Process %d could not be enqueued.\n", p.pid);
        return 0;  // failure
    }

    q->rear = (q->rear + 1) % MAX_PROCESSES;
    q->data[q->rear] = p;
    q->size++;
    return 1;  // success
}

PCB dequeue(Queue* q) {
    PCB p = q->data[q->front];
    q->front = (q->front + 1) % MAX_PROCESSES;
    q->size--;
    return p;
}

// Remove a specific index (used by SJF)
PCB removeAt(Queue* q, int index) {
    int realIndex = (q->front + index) % MAX_PROCESSES;
    PCB p = q->data[realIndex];

    for (int i = index; i < q->size - 1; i++) {
        int from = (q->front + i + 1) % MAX_PROCESSES;
        int to   = (q->front + i) % MAX_PROCESSES;
        q->data[to] = q->data[from];
    }

    q->rear = (q->rear - 1 + MAX_PROCESSES) % MAX_PROCESSES;
    q->size--;

    return p;
}

// ---------------- SCHEDULING ----------------

// FCFS
PCB scheduleFCFS(Queue* q) {
    return dequeue(q);
}
// ties are broken by: (1) arrival time — earlier arrival wins
//                     (2) then PID — lower PID wins
PCB scheduleSJF(Queue* q) {
    int index = 0;

    for (int i = 1; i < q->size; i++) {
        int current = (q->front + i) % MAX_PROCESSES;
        int best    = (q->front + index) % MAX_PROCESSES;

        int currentBurst   = q->data[current].remainingTime;
        int bestBurst      = q->data[best].remainingTime;
        int currentArrival = q->data[current].arrivalTime;
        int bestArrival    = q->data[best].arrivalTime;
        int currentPID     = q->data[current].pid;
        int bestPID        = q->data[best].pid;

        if (currentBurst < bestBurst) {
            // Clearly shorter — pick it
            index = i;
        } else if (currentBurst == bestBurst) {
            // Tie on burst — break by arrival time
            if (currentArrival < bestArrival) {
                index = i;
            } else if (currentArrival == bestArrival) {
                // Tie on arrival too — break by PID
                if (currentPID < bestPID) {
                    index = i;
                }
            }
        }
    }

    return removeAt(q, index);
}

// ---------------- SIMULATION ----------------

void simulate(int algorithm) {

    // Sample processes (you can modify these)
    PCB processes[] = {
        {0, 0, 8, 8, READY},
        {1, 1, 4, 4, READY},
        {2, 2, 9, 9, READY},
        {3, 3, 5, 5, READY}
    };

    int totalProcesses = 4;
    int completed = 0;
    int time = 0;

    Queue readyQueue;
    initQueue(&readyQueue);

    PCB* running = NULL;
    int quantumCounter = 0;

    printf("\n--- Simulation Start ---\n");

    while (completed < totalProcesses) {

        printf("\nTime %d:\n", time);

        // 1. Check arrivals
        for (int i = 0; i < totalProcesses; i++) {
            if (processes[i].arrivalTime == time) {
                printf("Process %d arrived\n", processes[i].pid);
                if (!enqueue(&readyQueue, processes[i])) {
                    printf("Warning: Process %d was lost due to full queue!\n",
                           processes[i].pid);
                }
            }
        }

        // 2. Dispatch if CPU is free
        if (running == NULL && !isEmpty(&readyQueue)) {

            running = (PCB*)malloc(sizeof(PCB));

            if (algorithm == 1) {
                *running = scheduleFCFS(&readyQueue);
                printf("Dispatching (FCFS) P%d\n", running->pid);
            }
            else if (algorithm == 2) {
                *running = scheduleSJF(&readyQueue);
                printf("Dispatching (SJF) P%d\n", running->pid);
            }
            else if (algorithm == 3) {
                *running = dequeue(&readyQueue); // Round Robin
                printf("Dispatching (RR) P%d\n", running->pid);
                quantumCounter = 0;
            }

            running->state = RUNNING;
        }

        // 3. Execute process
        if (running != NULL) {

            running->remainingTime--;
            printf("Running P%d (remaining: %d)\n",
                   running->pid, running->remainingTime);

            // Case 1: Process finished
            if (running->remainingTime == 0) {
                printf("Process %d finished\n", running->pid);
                running->state = FINISHED;
                free(running);
                running = NULL;
                completed++;
                quantumCounter = 0;
            }

            // Case 2: Round Robin time slice expires
            else if (algorithm == 3) {
                quantumCounter++;

                if (quantumCounter == TIME_QUANTUM) {
                    printf("Time slice expired for P%d\n", running->pid);

                    running->state = READY;
                    enqueue(&readyQueue, *running);

                    free(running);
                    running = NULL;
                    quantumCounter = 0;
                }
            }
        }

        time++;
    }

    printf("\n--- Simulation End ---\n");
}

// ---------------- MAIN ----------------

int main() {

    int choice;

    printf("Choose Scheduling Algorithm:\n");
    printf("1. FCFS\n");
    printf("2. SJF\n");
    printf("3. Round Robin\n");
    printf("Enter choice: ");

    scanf("%d", &choice);

    simulate(choice);

    return 0;
}
