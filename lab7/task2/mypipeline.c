#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFERLENGTH 2048


int pipe2(int pipefd[2], int flags);


int main(int argc, char **argv) {
  int in_child1 = 0;
  int in_child2 = 0;
  pid_t child1_pid;
  pid_t child2_pid;
  int debug = 0;

  for (int i = 1; i < argc; i++){
    if((strncmp(argv[i],"-d", 2) == 0)){
        debug = 1;
    }
  }

  //1.Create a pipe.
  int pipefd[2];
  if (pipe(pipefd) == -1) {
        perror("pipe");
        if(debug) fprintf(stderr, "(parent_process>exiting…)\n");
        exit(EXIT_FAILURE);
      }
  if(debug) fprintf(stderr, "(parent_process>forking…)\n");    
  //2.Fork a first child process (child1).
  child1_pid = fork();
  
  if (child1_pid == -1) {
      perror("fork");
      if(debug) fprintf(stderr, "(parent_process>exiting…)\n");
      exit(EXIT_FAILURE);
  }
  in_child1 = (child1_pid == 0);
  if(debug & !in_child1) fprintf(stderr, "(parent_process>created process with id: %d)\n", child1_pid);
  //3.In the child1 process:
  if(in_child1){
    //1.Close the standard output.
    close(STDOUT_FILENO);
    //2.Duplicate the write-end of the pipe using dup (see man).
    if(debug) fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe…)\n");
    dup2(pipefd[1], STDOUT_FILENO);
    //3.Close the file descriptor that was duplicated.
    close(pipefd[1]);
    //4.Execute "ls -l".
    char* args[] = {"ls", "-l", NULL}; // NULL terminated array of char* strings
    if(debug) fprintf(stderr, "(child1>going to execute cmd: …)\n");
    if(execvp("ls", args) == -1){
      perror("execvp() error");
      _exit(1);
    }   
  }
  else if(!in_child1 & !in_child2){
    //4.In the parent process: Close the write end of the pipe.
    if(debug) fprintf(stderr, "(parent_process>closing the write end of the pipe…)\n");
    close(pipefd[1]);
    //5.Fork a second child process (child2).
    if(debug) fprintf(stderr, "(parent_process>forking…)\n");
    child2_pid = fork();
    if (child2_pid == -1) {
      perror("fork");
      if(debug) fprintf(stderr, "(parent_process>exiting…)\n");
      exit(EXIT_FAILURE);
    }
    in_child2 = (child2_pid == 0);
    if(debug) fprintf(stderr, "(parent_process>created process with id: %d)\n", child2_pid);
  }
  //6.In the child2 process:
  if(in_child2){
    //1.Close the standard input.
    close(STDIN_FILENO);
    //2.Duplicate the read-end of the pipe using dup.
    if(debug) fprintf(stderr, "(child2>redirecting stdin to the write end of the pipe…)\n");
    dup2(pipefd[0], STDIN_FILENO);
    //3.Close the file descriptor that was duplicated.
    close(pipefd[0]);
    //4.Execute "tail -n 2".
    char* args[] = {"tail", "-n", "2", NULL};
    if(debug) fprintf(stderr, "(child2>going to execute cmd: …)\n");
    if(execvp("tail", args) == -1){
      perror("execvp() error");
      _exit(1);
    }   
  }
  //Both children are out, back to parent
  //7.In the parent process: Close the read end of the pipe.
  if(debug) fprintf(stderr, "(parent_process>closing the read end of the pipe…)\n");
  close(pipefd[0]);
  //if(debug) fprintf(stderr, "(parent_process>closing the write end of the pipe…)\n");
  //close(pipefd[1]);
  //8.Now wait for the child processes to terminate, in the same order of their execution.
  int status1;
  int status2;
  if(debug) fprintf(stderr, "(parent_process>waiting for child processes to terminate…)\n");
  pid_t w = waitpid(child1_pid, &status1, WUNTRACED | WCONTINUED);
  if (w == -1) {
    perror("waitpid");
    if(debug) fprintf(stderr, "(parent_process>exiting…)\n");
    exit(EXIT_FAILURE);
  }
  w = waitpid(child2_pid, &status2, WUNTRACED | WCONTINUED);
  if (w == -1) {
    perror("waitpid");
    if(debug) fprintf(stderr, "(parent_process>exiting…)\n");
    exit(EXIT_FAILURE);
  }  
  if(debug) fprintf(stderr, "(parent_process>exiting…)\n");

  return 0;
}
