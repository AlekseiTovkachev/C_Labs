#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>

void handler(int sig)
{
	printf("\nReceieved Signal : %s\n", strsignal(sig));
	if (sig == SIGTSTP){
		printf("Stopping...");
		}
	else if (sig == SIGCONT){
		printf("Continuing...");
	}
	else if (sig == SIGINT){
		printf("Interrupted...");
		}
	signal(sig, SIG_DFL);
	raise(sig);
	if(sig == SIGTSTP){
		signal(SIGCONT, handler);
	}
	if(sig == SIGCONT){
		signal(SIGTSTP, handler);
	}
}

int main(int argc, char **argv)
{

	printf("Starting the program\n");
	signal(SIGINT, handler);
	signal(SIGTSTP, handler);
	signal(SIGCONT, handler);

	while (1)
	{
		sleep(1);
	}

	return 0;
}