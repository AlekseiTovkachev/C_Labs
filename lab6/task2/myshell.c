#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include "LineParser.h"

#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

#define BUFFERLENGTH 2048


typedef struct process{
    cmdLine* cmd;                            /* the parsed command line*/
    pid_t pid; 		                        /* the process id that is running the command*/
    int status;                            /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	              /* next process in chain */
} process;

void freeProcessList(process* process_list){
    process * cur = process_list;
    while(cur!=NULL){
        process * temp = cur->next;
        cur = temp;
        freeCmdLines(cur);
    }
}


void updateProcessStatus(process* process_list, int pid, int status){
    if (WIFEXITED(status) | WIFSIGNALED(status)) {
      process_list->status = TERMINATED;
    }
    else if (WIFSTOPPED(status)) {
      process_list->status = SUSPENDED;
    }
    else if (WIFCONTINUED(status)) {
      process_list->status = RUNNING;
    } 
}

void updateProcessList(process **process_list){
    process * p = *process_list;
    while(p != NULL){
        int status;
        int res = waitpid(p->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
        if(res != 0){
          updateProcessStatus(p, p->pid, status);
        }    
        p = p->next;
    }
}


void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
  process * p = (process *) malloc(4*sizeof(int));
  p->cmd  = cmd;
  p->pid = pid;
  p->status = RUNNING;
  p->next = NULL;
  if(*process_list == NULL){
      *process_list = p;
      return;
  }
  process * cur = * process_list;
  process * next = cur->next;
  while(next!=NULL){
    cur = next;
    next = cur->next;
  }
  cur->next = p;
}

void printProcessList(process** process_list){

  process * newList = NULL;
  updateProcessList(process_list);
  process* cur = *process_list;

  while(cur!=NULL){
    printf("PID          Command          STATUS\n");
    printf("%d           %s               %d\n", cur->pid, (cur->cmd->arguments)[0], cur->status);
    if(cur->status != TERMINATED){
        addProcess(&newList, cur->cmd, cur->pid);
    }
    else{
        freeCmdLines(cur->cmd);
    }
    cur = cur->next;
  }
  process * temp = * process_list;
  * process_list = newList;
  freeProcessList(temp);
}




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
  process * processList = NULL;
  while(i < argc){
     if((strncmp(argv[i],"-d", 2) == 0)){
        debug = 1;
        }
        i++;           
  }
  char path[PATH_MAX];
  char line[BUFFERLENGTH];
  if (getcwd(path, sizeof(path)) == NULL)
    perror("getcwd() error");
   
  while(1){

    pid_t pid = getpid();
    printf("%s> ", path); 
    fgets(line, BUFFERLENGTH, stdin);

    if(strncmp("cd ", line, 3) == 0){

      char * p = &(line[3]);
      p[strlen(p)-1] = '\0';
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
    else if(strcmp("procs\n", line) == 0){
      printProcessList(&processList);
    }
    else if(strncmp("suspend", line, 7) == 0){
        pid_t p_id = atoi(line+8);
        kill(p_id, SIGTSTP);
    }
    else if(strncmp("kill", line, 4) == 0){
        pid_t p_id = atoi(line+5);
        kill(p_id, SIGINT);
    }
    else if(strncmp("wake", line, 4) == 0){
        pid_t p_id = atoi(line+5);
        kill(p_id, SIGCONT);
    }
    else{
 
    cmdLine * cmdl = parseCmdLines(line);
    pid_t cpid = fork();
    if (cpid == -1) {
        perror("fork");
        _exit(EXIT_FAILURE);
    }
    int child = (cpid == 0);
    if(child){
      if(debug){
        fprintf(stderr, "Executing command: %s", line);
        fprintf(stderr, "PID: %d\n", getpid());
      }
      execute(cmdl);
      exit(1); /* error! */
    }
    if(!child){
      addProcess(&processList, cmdl, cpid);
      if(cmdl->blocking != 0){
        pid_t w = waitpid(cpid, &status, WUNTRACED | WCONTINUED);
        if (w == -1) {
        perror("waitpid");
        _exit(EXIT_FAILURE);
        }
      }
    }
    }

  return 0;
}