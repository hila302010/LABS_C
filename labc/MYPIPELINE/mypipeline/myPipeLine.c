#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    pid_t cpid1, cpid2;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "(parent_process>forking…)\n");
    cpid1 = fork();
    if (cpid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid1 == 0) { // Child 1
        fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe…)\n");
        close(STDOUT_FILENO);
        dup(pipefd[1]);
        close(pipefd[1]);
        close(pipefd[0]);

        char *ls_args[] = {"ls", "-l", NULL};
        fprintf(stderr, "(child1>going to execute cmd: ls -l)\n");
        execvp("ls", ls_args);
        perror("execvp ls");
        exit(EXIT_FAILURE);
    } else {
        fprintf(stderr, "(parent_process>created process with id: %d)\n", cpid1);
        fprintf(stderr, "(parent_process>closing the write end of the pipe…)\n");
        close(pipefd[1]); // Close write end of the pipe in the parent

        fprintf(stderr, "(parent_process>forking…)\n");
        cpid2 = fork();
        if (cpid2 == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (cpid2 == 0) { // Child 2
            fprintf(stderr, "(child2>redirecting stdin to the read end of the pipe…)\n");
            close(STDIN_FILENO);
            dup(pipefd[0]);
            close(pipefd[0]);

            char *tail_args[] = {"tail", "-n", "2", NULL};
            fprintf(stderr, "(child2>going to execute cmd: tail -n 2)\n");
            execvp("tail", tail_args);
            perror("execvp tail");
            exit(EXIT_FAILURE);
        } else {
            fprintf(stderr, "(parent_process>created process with id: %d)\n", cpid2);
            fprintf(stderr, "(parent_process>closing the read end of the pipe…)\n");
            close(pipefd[0]); // Close read end of the pipe in the parent

            fprintf(stderr, "(parent_process>waiting for child processes to terminate…)\n");
            waitpid(cpid1, NULL, 0); // Wait for the first child
            waitpid(cpid2, NULL, 0); // Wait for the second child

            fprintf(stderr, "(parent_process>exiting…)\n");
        }
    }

    return EXIT_SUCCESS;
}
