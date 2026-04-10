#include <stdio.h>
#include <stdlib.h>
#include "Process.h"

const int READYQUEUESIZE = 10;

int FCFS(PCB* queue, int length){

    int index = -1;
    for(int i=0; i < length; i++){
        if(index < 0){
            // First instanceof smallest arrival time
            index = i;
        }else if(queue[i].arrivalTime < queue[index].arrivalTime){
            // Process arrived before the stored
            index = i;
        }else if(queue[i].arrivalTime  == queue[index].arrivalTime && queue[i].pid < queue[index].pid){
            // processes arrived simultaniously
            index = i;
        }
    }

    return index;
}

int SJF(PCB* queue, int length){
    int index = -1;
    int count = 0;

    for(int i=0; i < length; i++){
        if(index < 0)index = i;
        else if(queue[i].burstTime < queue[index].burstTime){
            index = i;
            count = 1;
        }else if(queue[i].burstTime == queue[index].burstTime){
            count++;
        }
    }
    
    if(count==1){
        return index;
    }
    int placeIndex = 0;
    PCB* subQueue = (PCB*)malloc(count*sizeof(PCB));

    for(int i=index; i < length; i++){
        if(queue[i].burstTime == queue[index].burstTime){
            subQueue[placeIndex] = queue[i];
            placeIndex++;
        }
    }
    int subIndex = FCFS(subQueue, count);
    
    for(int i=0; i < length; i++){
        if(queue[i].pid == subQueue[subIndex].pid){
            //free(subQueue);
            return i;
        }
    }
}


int main(){
    PCB* readyQueue = (PCB*)malloc(READYQUEUESIZE * sizeof(PCB));

    readyQueue[0] = (PCB){0, 200, 0, 200, READY};
    readyQueue[1] = (PCB){1, 100, 0, 50, READY};
    readyQueue[2] = (PCB){2, 100, 0, 50, READY};
    readyQueue[3] = (PCB){3, 100, 0, 50, READY};
    
    printf("Process is %d", SJF(readyQueue, READYQUEUESIZE));

    return 0;
}