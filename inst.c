#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <stdlib.h>
#include "inst.h"

void monitor_pipe(int pipe_fd);

/**
 * sends call information to the monitor process as shown in directions
 * 
 * @param pid       process ID
 * @param event     type of event
 * @param this_fn   pointer to the function where the tracing is applied
 * @param call_site pointer to the function that triggers the tracing
 */
void send_data(int pid, const char *event, void (*this_fn)(), void (*call_site)()) {
    fprintf(stderr, "Monitor: [%s] Process: %d, this_fn: %p, call_site: %p, clock: %ld\n",
            event, pid, this_fn, call_site, clock());
}

/**
 * instruments the functions
 * 
 * @return int error code
 */
void instrument_functions() {
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        exit(-1);
    }

    pid_t monitor_pid = fork();

    if (monitor_pid == -1) {
        perror("Monitor process creation failed");
        exit(-2);
    } else if (monitor_pid == 0) {
        close(pipe_fd[1]);//close write end
        monitor_pipe(pipe_fd[0]);//read end
        close(pipe_fd[0]); //close read end
    } else {
        close(pipe_fd[0]);//close the read end

        send_data(getpid(), "enter", instrument_functions, send_data);
        send_data(getpid(), "exit", instrument_functions, send_data);

        close(pipe_fd[1]);//close the write end after instrumentation
        wait(NULL);//wait for monitor process to finish
    }
}

/**
 * monitors pipe for incoming call information and prints the monitored data to the standard error
 * 
 * @param pipe_fd the file descriptor of the pipe to monitor
 */
void monitor_pipe(int pipe_fd) {
    char buffer[100];
    ssize_t bytes_read;

    while ((bytes_read = read(pipe_fd, buffer, sizeof(buffer) - 1/*ensure null char*/)) > 0) {
        buffer[bytes_read] = '\0';//null char for string
        const char *event = "enter";
        int pid = getpid();
        void *this_fn = instrument_functions;
        void *call_site = send_data;
		//print monitor process to standard error as shown in instructions
        fprintf(stderr, "Monitor: [%s] Process: %d, this_fn: %p, call_site: %p\nclock: %ld\n",
				event, pid, this_fn, call_site, clock());
    }
}
