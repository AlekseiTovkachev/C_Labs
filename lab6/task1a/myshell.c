#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "LineParser.h"


void execute(cmdLine *pCmdLine){
  const char * path = (pCmdLine->arguments)[0];
  if(execvp(path, pCmdLine->arguments) == -1){
      perror("execvp() error");
      _exit(1);
  }  
}

int main(int argc, char **argv) {
  int i = 1; 
  int debug = 0;
  int status;
  while(i < argc){
     if((strncmp(argv[i],"-d", 2) == 0)){
        debug = 1;
        }
        i++;           
  }
  char path[PATH_MAX];
  char line[2048];
  if (getcwd(path, sizeof(path)) == NULL)
    perror("getcwd() error");
   
  while(1){

    pid_t pid = getpid();
    printf("%s> ", path); 
    fgets(line, 2048, stdin);

    if(strncmp("cd ", line, 3) == 0){

      char * p = &(line[3]);
      p[strlen(p)-1] = '\0';
      //printf("%s", p);

      int res = chdir(p);

      if(res == -1){
        perror("chdir() error");
      }
      else{
        if (getcwd(path, sizeof(path)) == NULL)
          perror("getcwd() error");
      }

    }
    else if(strcmp("quit\n", line) == 0){
      exit(0);
    }
    else{
 
    cmdLine * cmdl = parseCmdLines(line);
    pid_t cpid = fork();

    if(debug & !cpid){
      fprintf(stderr, "Executing command: %s", line);
      fprintf(stderr, "PID: %d\n", getpid());
    }

    if((cmdl->blocking == 0) & (cpid > 0)){
      pid_t w = waitpid(cpid, &status, WUNTRACED | WCONTINUED);
      printf("in w now\n");
      if (w == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
      }
    } 

    if(!(pid = cpid)) {
      execute(cmdl);
      exit(1); /* error! */
    }

    free(cmdl);
    }
    
  }

  return 0;
}