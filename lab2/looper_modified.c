#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>

void handler(int sig)
{
	printf("\nRecieved Signal : %s\n", strsignal(sig));
	if (sig == SIGTSTP)
	{
		signal(SIGTSTP, SIG_DFL);
		raise(sig);
	}
	else if (sig == SIGCONT)
	{
		signal(SIGCONT, SIG_DFL);
		raise(sig);
	}
	//signal(sig, SIG_DFL);
}

int main(int argc, char **argv)
{
	/*int id1 = -1;
	id1 = fork();
	int id2 = fork();

	printf(" get id: %d\nfork id %d\n", getpid(), id1);

	int pid = waitpid(id2, NULL, 0);
	printf("waiting %d", pid);*/
	// printf("Starting the program\n");
	signal(SIGINT, handler);
	signal(SIGTSTP, handler);
	signal(SIGCONT, handler);

	// while (1)
	// {
	// 	sleep(1);
	// }*/

	return 0;
}