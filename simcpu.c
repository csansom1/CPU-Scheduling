/*
 * Filename: simcpu.c
 * Author: Connor Sansom
 * Course CIS 3110
 * Assignment: A2
 * Date: 27/02/17
 *
 * This program simulates FCFS and round robin CPU scheduling based on input from a text file.
 */

#ifndef CSANSOM_SIMCPU_C
#define CSANSOM_SIMCPU_C

#include "simcpu.h"

int main(int argc, char **argv) {
    int processCount, threadSwitch, processSwitch, detailed, verbose, rr, quantum;
    detailed = verbose = rr = quantum = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            detailed = 1;
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-r") == 0) {
            if (i == argc-1) {
                printf("Error: Please enter a quantum\n");
                exit(0);
            }
            rr = 1;
            quantum = atoi(argv[++i]);
            if (quantum == 0) {
                printf("Error: Quantum must be an integer > 0\n");
                exit(0);
            }
        } else {
            printf("Error: Invalid flag(s)\n");
            exit(0);
        }
    }

    char buffer[100];
    fgets(buffer, 100, stdin);
    sscanf(buffer, "%d %d %d", &processCount, &threadSwitch, &processSwitch);
    Process **processes = init(processCount);
    if (rr)
        roundRobin(processes, quantum, processCount, detailed, verbose, processSwitch, threadSwitch);
    else
        fcfs(processes, processCount, detailed, verbose, processSwitch, threadSwitch);
    free(processes);

    return 0;
}

Process **init(int processCount) {
    char buffer[100];
    Process **processes = malloc(sizeof(Process*) * processCount);
    for (int i = 0; i < processCount; i++) {
        processes[i] = malloc(sizeof(Process) + sizeof(int)*2 + sizeof(Thread**));
        fgets(buffer, 100, stdin);
        sscanf(buffer, "%d %d", &processes[i]->processNumber, &processes[i]->threadCount);
        processes[i]->threads = malloc(sizeof(Thread*) * processes[i]->threadCount);
        for (int j = 0; j < processes[i]->threadCount; j++) {
            processes[i]->threads[j] = malloc(sizeof(Thread) + sizeof(int)*11 + sizeof(CPU**));
            fgets(buffer, 100, stdin);
            sscanf(buffer, "%d %d %d", &processes[i]->threads[j]->threadNumber, &processes[i]->threads[j]->arrivalTime,
                     &processes[i]->threads[j]->cpuNumber);
            processes[i]->threads[j]->cpu = malloc(sizeof(CPU*) * processes[i]->threads[j]->cpuNumber);
            processes[i]->threads[j]->state = NEW;
            processes[i]->threads[j]->activeCPU = 0;
            processes[i]->threads[j]->serviceTime = 0;
            processes[i]->threads[j]->ioTime = 0;
            processes[i]->threads[j]->process = i + 1;
            processes[i]->threads[j]->start = processes[i]->threads[j]->arrivalTime;
            for (int k = 0; k < processes[i]->threads[j]->cpuNumber; k++) {
                processes[i]->threads[j]->cpu[k] = malloc(sizeof(CPU) + sizeof(int)*3);
                fgets(buffer, 100, stdin);
                if (k == processes[i]->threads[j]->cpuNumber-1) {
                    sscanf(buffer, "%d %d", &processes[i]->threads[j]->cpu[k]->cpuNumber,
                             &processes[i]->threads[j]->cpu[k]->cpuTime);
                    processes[i]->threads[j]->cpu[k]->ioTime = 0;
                } else
                    sscanf(buffer, "%d %d %d", &processes[i]->threads[j]->cpu[k]->cpuNumber,
                             &processes[i]->threads[j]->cpu[k]->cpuTime, &processes[i]->threads[j]->cpu[k]->ioTime);
            }
        } 
    }
    return processes;
}

void fcfs(Process **processes, int processCount, int detailed, int verbose, int processSwitch, int threadSwitch) {
    Thread *ready[10000];
    int time, queueStart, queueEnd, numThreads, idleTime;
    time = queueStart = queueEnd = numThreads = idleTime = 0;
    double avgTA, cpuUtil;
    avgTA = 0.0;
    printf("FCFS:\n\n");
    for (int i = 0; i < processCount; i++) {
        numThreads += processes[i]->threadCount;
        for (int j = 0; j < processes[i]->threadCount; j++) {
            ready[queueEnd] = processes[i]->threads[j];
            queueEnd++;
            int k = queueEnd - 1;
            while (queueEnd > 1 && ready[k]->arrivalTime < ready[k-1]->arrivalTime) {
                Thread *temp = ready[k-1];
                ready[k-1] = ready[k];
                ready[k] = temp;
                k--;
            }
        }
    }
    for (int i = queueStart; i < queueEnd; i++) {
        ready[i]->state = READY;
        if (verbose) {
            printf("At time %d: Thread %d of Process %d moves from NEW to READY\n\n", ready[i]->arrivalTime,
                    ready[i]->threadNumber, ready[i]->process);
        }
    }
    int index = 0;
    while (queueStart < queueEnd) {
        if (time < ready[index]->start) {
            idleTime += ready[index]->start - time;
            time += ready[index]->start - time;
        }
        if (verbose) {
            if (ready[index]->start != ready[index]->arrivalTime)
                printf("At time %d: Thread %d of Process %d moves from BLOCKED to READY\n\n", ready[index]->start,
                         ready[index]->threadNumber, ready[index]->process);
            printf("At time %d: Thread %d of Process %d moves from READY to RUNNING\n\n", time,
                      ready[index]->threadNumber, ready[index]->process);
        }
        ready[index]->state = RUNNING;
        time += ready[index]->cpu[ready[index]->activeCPU]->cpuTime;
        ready[index]->serviceTime += ready[index]->cpu[ready[index]->activeCPU]->cpuTime;
        ready[index]->ioTime += ready[index]->cpu[ready[index]->activeCPU]->ioTime;
        ready[index]->start = time + ready[index]->ioTime;
        ready[index]->activeCPU++;
        if (ready[index]->activeCPU == ready[index]->cpuNumber) {
            ready[index]->state = TERMINATED;
            if (verbose)
                printf("At time %d: Thread %d of Process %d moves from RUNNING to TERMINATED\n\n", time,
                          ready[index]->threadNumber, ready[index]->process);
            ready[index]->end = time;
            ready[index]->taTime = ready[index]->end - ready[index]->arrivalTime;
            avgTA += (double) ready[index]->taTime;
        } else {
            ready[index]->state = BLOCKED;
            if (verbose)
                printf("At time %d: Thread %d of Process %d moves from RUNNING to BLOCKED\n\n", time,
                          ready[index]->threadNumber, ready[index]->process);
            ready[queueEnd++] = ready[index];
            int k = queueEnd - 1;
            while (queueEnd > 1 && ready[k]->start < ready[k-1]->start) {
                Thread *temp = ready[k-1];
                ready[k-1] = ready[k];
                ready[k] = temp;
                k--;
            }
        }
        if (index < queueEnd - 1) {
            if (ready[index]->process != ready[index+1]->process)
                time += processSwitch;
            else if (ready[index]->threadNumber != ready[index+1]->threadNumber)
                time += threadSwitch;
        }
        queueStart++;
        index++;
    }
    avgTA /= ((double) numThreads);
    cpuUtil = ((double) (time - idleTime)) / ((double) time) * 100.0;
    printf("Total Time: %d time units\nAgerage Turnaround Time: %lf time units\nCPU Utilization %lf%%\n\n", time, avgTA, cpuUtil);
    if (detailed) {
        for (int i = 0; i < processCount; i++) {
            for (int j = 0; j < processes[i]->threadCount; j++) {
                printf("Thread %d of Process %d:\n\n", processes[i]->threads[j]->threadNumber,
                          processes[i]->threads[j]->process);
                printf("arrival time: %d\n", processes[i]->threads[j]->arrivalTime);
                printf("service time: %d\n", processes[i]->threads[j]->serviceTime);
                printf("I/O time: %d\n", processes[i]->threads[j]->ioTime);
                printf("turnaround time: %d\n", processes[i]->threads[j]->taTime);
                printf("finish time: %d\n\n", processes[i]->threads[j]->end);
            }
        }
    }
}

void roundRobin(Process **processes, int quantum, int processCount, int detailed, int verbose, int processSwitch, int threadSwitch){
     Thread *ready[10000];
    int time, queueStart, queueEnd, numThreads, idleTime, quantumTime;
    time = queueStart = queueEnd = numThreads = idleTime = 0;
    quantumTime = quantum;
    double avgTA, cpuUtil;
    avgTA = 0.0;
    printf("Round Robin (quantum = %d time units):\n\n", quantum);
    for (int i = 0; i < processCount; i++) {
        numThreads += processes[i]->threadCount;
        for (int j = 0; j < processes[i]->threadCount; j++) {
            ready[queueEnd] = processes[i]->threads[j];
            queueEnd++;
            int k = queueEnd - 1;
            while (queueEnd > 1 && ready[k]->arrivalTime < ready[k-1]->arrivalTime) {
                Thread *temp = ready[k-1];
                ready[k-1] = ready[k];
                ready[k] = temp;
                k--;
            }
        }
    }
    for (int i = queueStart; i < queueEnd; i++) {
        ready[i]->state = READY;
        if (verbose) {
            printf("At time %d: Thread %d of Process %d moves from NEW to READY\n\n", ready[i]->arrivalTime,
                ready[i]->threadNumber, ready[i]->process);
        }
    }
    int index = 0;
    while (queueStart < queueEnd) {
        if (time < ready[index]->start) {
            idleTime += ready[index]->start - time;
            time += ready[index]->start - time;
        }
        if (verbose) {
            if (ready[index]->start != ready[index]->arrivalTime)
                printf("At time %d: Thread %d of Process %d moves from BLOCKED to READY\n\n", ready[index]->start,
                         ready[index]->threadNumber, ready[index]->process);
            printf("At time %d: Thread %d of Process %d moves from READY to RUNNING\n\n", time,
                      ready[index]->threadNumber, ready[index]->process);
        }
        ready[index]->state = RUNNING;
        if (ready[index]->cpu[ready[index]->activeCPU]->cpuTime - quantumTime >= 0) {
            time += quantumTime;
            ready[index]->serviceTime += quantumTime;
            ready[index]->cpu[ready[index]->activeCPU]->cpuTime -= quantumTime;
            ready[index]->start = time;
            quantumTime = (time % quantum == 0)? quantum : (ready[index]->cpu[ready[index]->activeCPU]->cpuTime - quantumTime);
        } else {
            time += ready[index]->cpu[ready[index]->activeCPU]->cpuTime;
            ready[index]->serviceTime += ready[index]->cpu[ready[index]->activeCPU]->cpuTime;
            ready[index]->start += ready[index]->ioTime;
            quantumTime = quantumTime - ready[index]->cpu[ready[index]->activeCPU]->cpuTime;
            ready[index]->cpu[ready[index]->activeCPU]->cpuTime = 0;
        }
        if (ready[index]->cpu[ready[index]->activeCPU]->cpuTime == 0) {
            ready[index]->ioTime += ready[index]->cpu[ready[index]->activeCPU]->ioTime;
            ready[index]->activeCPU++;
        }
        if (ready[index]->activeCPU == ready[index]->cpuNumber) {
            ready[index]->state = TERMINATED;
            if (verbose)
                printf("At time %d: Thread %d of Process %d moves from RUNNING to TERMINATED\n\n", time,
                          ready[index]->threadNumber, ready[index]->process);
            ready[index]->end = time;
            ready[index]->taTime = ready[index]->end - ready[index]->arrivalTime;
            avgTA += (double) ready[index]->taTime;
        } else {
            ready[index]->state = BLOCKED;
            if (verbose)
                printf("At time %d: Thread %d of Process %d moves from RUNNING to BLOCKED\n\n", time,
                          ready[index]->threadNumber, ready[index]->process);
            ready[queueEnd++] = ready[index];
            int k = queueEnd - 1;
            while (queueEnd > 1 && ready[k]->start < ready[k-1]->start) {
                Thread *temp = ready[k-1];
                ready[k-1] = ready[k];
                ready[k] = temp;
                k--;
            }
        }
        if (index < queueEnd - 1) {
            if (ready[index]->process != ready[index+1]->process)
                time += processSwitch;
            else if (ready[index]->threadNumber != ready[index+1]->threadNumber)
                time += threadSwitch;
        }
        queueStart++;
        index++;
    }
    avgTA /= ((double) numThreads);
    cpuUtil = ((double) (time - idleTime)) / ((double) time) * 100.0;
    printf("Total Time: %d time units\nAgerage Turnaround Time: %lf time units\nCPU Utilization %lf%%\n\n", time, avgTA, cpuUtil);
    if (detailed) {
        for (int i = 0; i < processCount; i++) {
            for (int j = 0; j < processes[i]->threadCount; j++) {
                printf("Thread %d of Process %d:\n\n", processes[i]->threads[j]->threadNumber,
                          processes[i]->threads[j]->process);
                printf("arrival time: %d\n", processes[i]->threads[j]->arrivalTime);
                printf("service time: %d\n", processes[i]->threads[j]->serviceTime);
                printf("I/O time: %d\n", processes[i]->threads[j]->ioTime);
                printf("turnaround time: %d\n", processes[i]->threads[j]->taTime);
                printf("finish time: %d\n\n", processes[i]->threads[j]->end);
            }
        }
    }
}

void freeProcesses(Process **processes, int processCount) {
    for (int i = 0; i < processCount; i++) {
        for (int j = 0; j < processes[i]->threadCount; j++) {
            for (int k = 0; k < processes[i]->threads[j]->cpuNumber; k++)
                free(processes[i]->threads[j]->cpu[k]);
            free(processes[i]->threads[j]->cpu);
            free(processes[i]->threads[j]);
        }
        free(processes[i]->threads);
        free(processes[i]);
    }
    free(processes);
}

#endif
