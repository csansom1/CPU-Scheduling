/*
 * Filename: simcpu.h
 * Author: Connor Sansom
 * Course CIS 3110
 * Assignment: A2
 * Date: 27/02/17
 *
 * This header file provides structs, functions, and enums for the purpose
 * of simulating FCFS and round robin CPU scheduling.
 */

#ifndef CSANSOM_SIMCPU_H
#define CSANSOM_SIMCPU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// All possible thread states.
typedef enum {NEW, READY, RUNNING, BLOCKED, TERMINATED} State;

/*
 * Represents a step of a process thread.
 * cpuNumber - the step number
 * cpuTime - time required for CPU to execute this step
 * ioTime - the I/O response time for this step
 */
typedef struct CPU {
    int cpuNumber, cpuTime, ioTime;
} CPU;

/*
 * A process thread.
 * threadNumber - the thread number
 * arrivalTime - time at wchich the thread arrives
 * cpuNumber - number of cpu steps (CPU time and I/O time)
 * activeCPU - the current step
 * process - the process this thread is a part of
 * serviceTime - the total service time
 * ioTime - the total I/O time
 * taTime - the turnaround time
 * start - the arrival time when the process goes back in the ready queue
 * end - time at which the process ends
 * cpu - a list of cpu steps (includes CPU time and I/O time)
 * state - the current state
 */
typedef struct Thread {
    int threadNumber, arrivalTime, cpuNumber, activeCPU, process, serviceTime, ioTime, taTime, start, end;
    CPU **cpu;
    State state;
} Thread;

/*
 * A process.
 * processNumber - the process number
 * threadCount - number of threads this process has
 * threads - a list of this process' threads
 */
typedef struct Process {
    int processNumber, threadCount;
    Thread **threads;
} Process;

/*
 * Reads in input from a text file.
 * IN: processCount - number of processs
 * OUT: a list of processes from the text file
 * PRE: the text file must be formatted correctly
 * POST: the process list is malloced inside init(), freeProcesses() must be called later in the program
 */
Process **init(int processCount);

/*
 * Initialize ready queue and perform CPU scheduling using first come first serve.
 * IN: processes - the list of processes
 *     processCount - length of the process list
 *     detailed - detailed mode (1 or 0)
 *     verbose - verbose mode (1 or 0)
 *     processSwitch - time taken to switch processes
 *     threadSwitch - time taken to switch threads
 * OUT: void
 * PRE: - processes is not NULL
 *      - - processCount is the length of processes
 * POST: FCFS scheduling is performed and relivent information is printed to stdout
 */
void fcfs(Process **processes, int processCount, int detailed, int verbose, int processSwitch, int threadSwitch);

/*
 * Initialize ready queue and perform CPU scheduling using round robin with the specified quantum.
 * IN: processes - the list of processes
 *     quantum - the time quantum
 *     processCount - length of the process list
 *     detailed - detailed mode (1 or 0)
 *     verbose - verbose mode (1 or 0)
 *     processSwitch - time taken to switch processes
 *     threadSwitch - time taken to switch threads
 * OUT: void
 * PRE: - processes is not NULL
 *      - quantum must be > 0
 *      - processCount is the length of processes
 * POST:RR scheduling is performed with the specified quantum  and relivent information is printed to stdout
 */
void roundRobin(Process **processes, int quantum, int processCount, int detailed, int verbose, int processSwitch, int threadSwitch);

/*
 * Free a list of processes.
 * IN: processes - the list of processes
 *     processCount - length of the process list
 * OUT: void
 * PRE: - processes has been malloced
        - processCount is the length of processes
 * POST: all memory alocated for processes has been freed
*/
void freeProcesses(Process **processes, int processCount);

#endif
