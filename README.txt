Name: Connor Sansom
ID: 0871933
Date: 27/02/2017
Course: CIS 3110
Assignment: A2

**********
*Overview*
**********

simcpu simulates CPU scheduling using either a first come first serve (FCFS) or round robin (RR) algorithm, based on input from a text file.
Context switching is used the scheduler switches between threads or processes. This includes when the CPU is idle and when a process is
preempted, but does not include the first switch from ready to running or when the ready queue is empty. Additionally, time used to switch
between processes or threads does not count towards the quantum time in RR mode, (i.e. if the CPU switches between processes or threads
the next event executes for the full quantum.


***********
*Compiling*
***********

To compile the shell, "cd" into the a1 directory and type "make".
- requires make, and gcc,
- required files: simcpu.c, simcpu.h, Makefile

**********
*Executon*
**********

To start the shell, type "./simcpu < <filename>" in bash. This executes simcpu with the defualt settings:
- FCFS scheduling
- display basice information: - Total time
                              - Average turnarround time
                              - % CPU utilization

simcpu allows the use of the following flags, these flags must appear between "./simcpu" and "< <filename>", eg "./simcpu -v < file.txt":

	"-r <quantum>": RR scheduling with the specified quantum

	"-d": detailed information
	      - display arrival time, service time, I/O time, turnaround time, and finish time of each thread

	"-v": verbose mode
	      - display all state changes of each thread and the time at which they occur

*******************
*Known Limitations*
*******************

	- simcpu does not check for errors in the text file
	  - make sure it is formatted correctly (comments at the end of lines are acceptable)
	- threads do not run concurrently, they instead run one after another
	- times desplayed in verbose mode are sometimes incorrect in RR mode
