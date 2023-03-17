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
      exit(1);
  }  
}



int main(int argc, char **argv) {
  char path[PATH_MAX];
  char line[2048];
  if (getcwd(path, sizeof(path)) == NULL)
    perror("getcwd() error");
   
  while(1){
    printf("%s> ", path); 
    fgets(line, 2048, stdin);
    if(strcmp("quit\n", line) == 0){
      exit(0);
    }
    cmdLine * cmdl = parseCmdLines(line); 
    execute(cmdl);
    free(cmdl);
  }

  return 0;
}



