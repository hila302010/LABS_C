 #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "LineParser.h"
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

#define MAX_INPUT_SIZE 2048
#define HISTLEN 20
#define MAX_BUF 200



typedef struct process {
    cmdLine* cmd;          /* the parsed command line */
    pid_t pid;             /* the process id that is running the command */
    int status;            /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;  /* next process in chain */
} process;


// Functions signatures
void execute(cmdLine *pCmdLine);
void handler(int sig);
void addProcess(process** process_list, cmdLine* cmd, pid_t pid);
void printProcessList(process** process_list);
void freeProcessList(process* process_list);
void updateProcessList(process **process_list);
void updateProcessStatus(process* process_list, int pid, int status);
void addToHistory(char* command);
void printHistory();
char* getCommandFromHistory(int index);

int debugMode = 0;
process *process_list = NULL;
char history[HISTLEN][MAX_BUF];
int newest = 0;
int oldest = 0;
int numEntries = 0;

#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0
        

int main(int argc, char *argv[]) {
    char cwd[PATH_MAX];//cwd is a character array where the current working directory path will be stored.
    char input[MAX_INPUT_SIZE];
    cmdLine *parsedLine;

    signal(SIGINT, handler);
    signal(SIGTSTP, handler);
    signal(SIGCONT, handler);

    // Check if the shell was started with the -d flag
    // maybe needs to be checked in loop
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        debugMode = 1;
    }

    while (1) {
        // Display the prompt
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s#> ", cwd);
        } else {
            perror("getcwd");
            exit(1);
        }

        // Read input from the user
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("fgets");
            continue;
        }

        // Remove trailing newline character
        input[strcspn(input, "\n")] = 0;

        // Parse the input
        parsedLine = parseCmdLines(input);
        if (parsedLine == NULL) {
            continue;
        }

        // Handle internal commands in execute
        execute(parsedLine);

        addToHistory(input);

    }
    freeProcessList(process_list);
    return 0;
}


// Function to add a command to the history
void addToHistory(char* command) {
    // handle the command !!
    if (strcmp(command, "!!") == 0) {
        char* lastCommand = getCommandFromHistory(numEntries);
        if (lastCommand != NULL) {
            strncpy(history[newest], lastCommand, MAX_BUF - 1);
            history[newest][MAX_BUF - 1] = '\0';
            newest = (newest + 1) % HISTLEN;
            if (numEntries < HISTLEN) {
                numEntries++;
            } else {
                oldest = (oldest + 1) % HISTLEN;
            }
        } else {
            printf("No previous command\n");
        }
    // handle the command !number
    } else if (command[0] == '!' ) {
        int index = atoi(command + 1);
        char* historyCommand = getCommandFromHistory(index);
        if (historyCommand != NULL) {
            strncpy(history[newest], historyCommand, MAX_BUF - 1);
            history[newest][MAX_BUF - 1] = '\0';
            newest = (newest + 1) % HISTLEN;
            if (numEntries < HISTLEN) {
                numEntries++;
            } else {
                oldest = (oldest + 1) % HISTLEN;
            }
        } else {
            printf("Invalid history index\n");
        }
    // handle all the rest of the commands
    } else {
        strncpy(history[newest], command, MAX_BUF - 1);
        history[newest][MAX_BUF - 1] = '\0';
        newest = (newest + 1) % HISTLEN;
        if (numEntries < HISTLEN) {
            numEntries++;
        } else {
            oldest = (oldest + 1) % HISTLEN;
        }
    }
}

// Function to print the history
void printHistory() {
    int i = oldest;
    int count = 1;
    do {
        printf("%d: %s\n", count++, history[i]);
        i = (i + 1) % HISTLEN;
    } while (i != newest);
}

// Function to retrieve a command from history by index
char* getCommandFromHistory(int index) {
    if (index < 1 || index > numEntries) {
        printf("Invalid history index\n");
        return NULL;
    }
    int i = oldest + index - 1;
    if (i >= HISTLEN) {
        i -= HISTLEN;
    }
    return history[i];
}

// add proccess to the process_list
void addProcess(process** process_list, cmdLine* cmd, pid_t pid) {
    process *newProcess = (process*)malloc(sizeof(process));
    newProcess->cmd = cmd;
    newProcess->pid = pid;
    newProcess->status = RUNNING;
    newProcess->next = *process_list;
    *process_list = newProcess;
}

// update proccess status in the process_list
void updateProcessStatus(process* process_list, int pid, int status) {
    while (process_list != NULL) {
        if (process_list->pid == pid) {
            process_list->status = status;
            break;
        }
        process_list = process_list->next;
    }
}


void updateProcessList(process **process_list) {
    int status;
    process *curr = *process_list;
    while (curr != NULL) {
        int result = waitpid(curr->pid, &status, WNOHANG | WUNTRACED);
        if (result == 0) {
            // Process is still running
            curr->status = RUNNING;
        } else if (result == -1) {
            // Error occurred
            perror("waitpid");
        } else {
            // Process has changed state
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                curr->status = TERMINATED;
            } else if (WIFSTOPPED(status)) {
                curr->status = SUSPENDED;
            } else if (WIFCONTINUED(status)) {
                curr->status = RUNNING;
            }
        }
        curr = curr->next;
    }
}


void freeProcessList(process* process_list) {
    while (process_list != NULL) {
        process *next = process_list->next;
        freeCmdLines(process_list->cmd);
        free(process_list);
        process_list = next;
    }
}


void printProcessList(process** process_list) {
    updateProcessList(process_list);
    process *curr = *process_list;
    process *prev = NULL;

    printf("PID\t\tCommand\t\tSTATUS\n");
    while (curr != NULL) {
        char *statusStr;
        switch (curr->status) {
            case RUNNING: statusStr = "Running"; break;
            case SUSPENDED: statusStr = "Suspended"; break;
            case TERMINATED: statusStr = "Terminated"; break;
            default: statusStr = "Unknown"; break;
        }
        printf("%d\t\t%s\t\t%s\n", curr->pid, curr->cmd->arguments[0], statusStr);

        if (curr->status == TERMINATED) {
            if (prev == NULL) {
                *process_list = curr->next;
                freeCmdLines(curr->cmd);
                free(curr);
                curr = *process_list;
            } else {
                prev->next = curr->next;
                freeCmdLines(curr->cmd);
                free(curr);
                curr = prev->next;
            }
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}



void execute(cmdLine *pCmdLine) {
    extern process *process_list;
    int pipefd[2];
    pid_t pid1, pid2;

    // Handle internal commands
    if (strcmp(pCmdLine->arguments[0], "quit") == 0) {
        exit(0);
    } else if (strcmp(pCmdLine->arguments[0], "cd") == 0) {
        if (pCmdLine->argCount < 2) {
            fprintf(stderr, "cd: missing argument\n");
        } else {
            if (chdir(pCmdLine->arguments[1]) != 0) {
                perror("cd");
            }
        }
        return;
    } else if (strcmp(pCmdLine->arguments[0], "alarm") == 0) {
        if (pCmdLine->argCount < 2) {
            fprintf(stderr, "alarm: missing process id\n");
        } else {
            int pid = atoi(pCmdLine->arguments[1]);
            if (kill(pid, SIGCONT) == 0) {
                printf("Sent SIGCONT to process %d\n", pid);
            } else {
                perror("alarm");
            }
        }
        return;
    } else if (strcmp(pCmdLine->arguments[0], "blast") == 0) {
        if (pCmdLine->argCount < 2) {
            fprintf(stderr, "blast: missing process id\n");
        } else {
            int pid = atoi(pCmdLine->arguments[1]);
            if (kill(pid, SIGINT) == 0) {
                printf("Sent SIGINT to process %d\n", pid);
            } else {
                perror("blast");
            }
        }
        return;
    } else if (strcmp(pCmdLine->arguments[0], "sleep") == 0) {
        if (pCmdLine->argCount < 2) {
            fprintf(stderr, "sleep: missing process id\n");
        } else {
            int pid = atoi(pCmdLine->arguments[1]);
            if (kill(pid, SIGTSTP) == 0) {
                printf("Sent SIGTSTP to process %d\n", pid);
            } else {
                perror("sleep");
            }
        }
        return;
    } else if (strcmp(pCmdLine->arguments[0], "procs") == 0) {
        printProcessList(&process_list);
        return;
    } else if (strcmp(pCmdLine->arguments[0], "wake") == 0) {
        if (pCmdLine->argCount < 2) {
            fprintf(stderr, "wake: missing process id\n");
        } else {
            int pid = atoi(pCmdLine->arguments[1]);
            updateProcessStatus(process_list, pid, RUNNING);
            kill(pid, SIGCONT);
        }
        return;
    } else if (strcmp(pCmdLine->arguments[0], "suspend") == 0) {
        if (pCmdLine->argCount < 2) {
            fprintf(stderr, "suspend: missing process id\n");
        } else {
            int pid = atoi(pCmdLine->arguments[1]);
            updateProcessStatus(process_list, pid, SUSPENDED);
            kill(pid, SIGTSTP);
        }
        return;
    } else if (strcmp(pCmdLine->arguments[0], "kill") == 0) {
        if (pCmdLine->argCount < 2) {
            fprintf(stderr, "kill: missing process id\n");
        } else {
            int pid = atoi(pCmdLine->arguments[1]);
            if (kill(pid, SIGKILL) == 0) {
                printf("Sent SIGKILL to process %d\n", pid);
            } else {
                perror("kill");
            }
        }
        return;
    } else if (strcmp(pCmdLine->arguments[0], "history") == 0) {
        printHistory();
        return;
    } else if (strcmp(pCmdLine->arguments[0], "!!") == 0) {
        char* lastCommand = getCommandFromHistory(numEntries);
        if (lastCommand != NULL) {
            printf("Executing: %s\n", lastCommand);
            // Parse the command and execute it
            cmdLine* parsedLastCommand = parseCmdLines(lastCommand);
            execute(parsedLastCommand);
        }
        return;
    } else if (pCmdLine->arguments[0][0] == '!') {
        int index = atoi(pCmdLine->arguments[0] + 1);
        char* command = getCommandFromHistory(index);
        if (command != NULL) {
            printf("Executing: %s\n", command);
            // Parse the command and execute it
            cmdLine* parsedCommand = parseCmdLines(command);
            execute(parsedCommand);
            return;
        }
    }
    if (pCmdLine->next) { // Check if there is a pipe
        if (pipe(pipefd) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        // First child process
        pid1 = fork();
        if (pid1 == 0) {
            addProcess(&process_list, pCmdLine, pid1);
            // Handle input redirection for the first command in the pipeline
            if (pCmdLine->inputRedirect) {
                int in = open(pCmdLine->inputRedirect, O_RDONLY);
                if (in < 0) {
                    perror("Error opening input file");
                    exit(EXIT_FAILURE);
                }
                dup2(in, STDIN_FILENO);
                close(in);
            }

            close(pipefd[0]); // Close read end of the pipe
            dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe write end
            close(pipefd[1]); // Close write end after dup2

            if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }

        // Second child process
        pid2 = fork();
        if (pid2 == 0) {
            addProcess(&process_list, pCmdLine->next, pid2);
            // Handle output redirection for the second command in the pipeline
            if (pCmdLine->next->outputRedirect) {
                int out = open(pCmdLine->next->outputRedirect, O_RDWR | O_CREAT, 0777);
                if (out < 0) {
                    perror("Error opening output file");
                    exit(EXIT_FAILURE);
                }
                dup2(out, STDOUT_FILENO);
                close(out);
            }

            close(pipefd[1]); // Close write end of the pipe
            dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to pipe read end
            close(pipefd[0]); // Close read end after dup2

            if (execvp(pCmdLine->next->arguments[0], pCmdLine->next->arguments) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }
        // Parent process
        close(pipefd[0]);
        close(pipefd[1]);
    } else {
        pid_t child_pid = fork();
        if (child_pid == 0) {
            // Handle input redirection
            if (pCmdLine->inputRedirect) {
                int in = open(pCmdLine->inputRedirect, O_RDONLY);
                if (in < 0) {
                    perror("Error opening input file");
                    exit(EXIT_FAILURE);
                }
                dup2(in, STDIN_FILENO);
                close(in);
            }

            // Handle output redirection
            if (pCmdLine->outputRedirect) {
                int out = open(pCmdLine->outputRedirect, O_RDWR | O_CREAT, 0777);
                if (out < 0) {
                    perror("Error opening output file");
                    exit(EXIT_FAILURE);
                }
                dup2(out, STDOUT_FILENO);
                close(out);
            }

            // Execute the command
            if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
                perror("Error executing command");
                _exit(EXIT_FAILURE);
            }
        } else if (child_pid < 0) {
            perror("Error forking");
            exit(EXIT_FAILURE);
        } else {
            addProcess(&process_list, pCmdLine, child_pid);
        }
    }
}


void handler(int sig)
{
	printf("\nRecieved Signal : %s\n", strsignal(sig));
	if (sig == SIGTSTP)
	{
		signal(SIGTSTP, SIG_DFL);
        raise(SIGTSTP);
	}
	else if (sig == SIGCONT)
	{
		signal(SIGCONT, SIG_DFL);
        raise(SIGCONT);
	}
}
