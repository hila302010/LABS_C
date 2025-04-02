#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {
    int pipe_fd[2]; // File descriptors for the pipe
    int pid;
    char write_msg[] = "hello";
    char read_msg[100];

    // Create the pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return 1;
    }

    // Fork the process
    pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;

    } else if (pid == 0) { // Child process
        close(pipe_fd[0]); // Close the read end of the pipe
        write(pipe_fd[1], write_msg, strlen(write_msg) + 1); // Write the message to the pipe
        close(pipe_fd[1]); // Close the write end of the pipe
        
    } else { // Parent process
        close(pipe_fd[1]); // Close the write end of the pipe
        read(pipe_fd[0], read_msg, sizeof(read_msg)); // Read the message from the pipe
        printf("Received message: %s\n", read_msg); // Print the message
        close(pipe_fd[0]); // Close the read end of the pipe
    }

    return 0;
}
