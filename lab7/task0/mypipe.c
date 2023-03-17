#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>



int main(int argc, char **argv) {
    int pipefd[2];
    pid_t cpid;
    char buf;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
 
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    int child = (cpid == 0);
    if(child){
        close(pipefd[0]);          /* Close unused read end */
        write(pipefd[1], "hello", 5);
        close(pipefd[1]);          /* Reader will see EOF */
        exit(EXIT_SUCCESS);
    }
    else if(!child){
        wait(NULL); 
        close(pipefd[1]);          /* Close unused write end */
        while (read(pipefd[0], &buf, 1) > 0)
            write(STDOUT_FILENO, &buf, 1);
        write(STDOUT_FILENO, "\n", 1);
        close(pipefd[0]);
        _exit(EXIT_SUCCESS);
    }

    return 0;
}