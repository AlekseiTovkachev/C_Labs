#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "LineParser.h"

#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0

#define BUFFERLENGTH 2048
#define HISTLEN 5

int pipe2(int pipefd[2], int flags);

typedef struct process
{
  cmdLine *cmd;         /* the parsed command line*/
  pid_t pid;            /* the process id that is running the command*/
  int status;           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
  struct process *next; /* next process in chain */
} process;

void freeProcessList(process *process_list)
{
  process *cur = process_list;
  while (cur != NULL)
  {
    process *temp = cur->next;
    cur = temp;
    // freeCmdLines(cur->cmd);
  }
}

void freeAllProcessList(process *process_list)
{
  process *cur = process_list;
  while (cur != NULL)
  {
    process *temp = cur->next;
    cur = temp;
    freeCmdLines(cur->cmd);
  }
}

void updateProcessStatus(process *process_list, int pid, int status)
{
  if (WIFEXITED(status) | WIFSIGNALED(status))
  {
    process_list->status = TERMINATED;
  }
  else if (WIFSTOPPED(status))
  {
    process_list->status = SUSPENDED;
  }
  else if (WIFCONTINUED(status))
  {
    process_list->status = RUNNING;
  }
}

void updateProcessList(process **process_list)
{
  process *p = *process_list;
  while (p != NULL)
  {
    int status;
    int res = waitpid(p->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
    if (res != 0)
    {
      updateProcessStatus(p, p->pid, status);
    }
    p = p->next;
  }
}

void addProcess(process **process_list, cmdLine *cmd, pid_t pid)
{
  process *p = (process *)malloc(4 * sizeof(int));
  p->cmd = cmd;
  p->pid = pid;
  p->status = RUNNING;
  p->next = NULL;
  if (*process_list == NULL)
  {
    *process_list = p;
    return;
  }
  process *cur = *process_list;
  process *next = cur->next;
  while (next != NULL)
  {
    cur = next;
    next = cur->next;
  }
  cur->next = p;
}

void printProcessList(process **process_list)
{

  process *newList = NULL;
  updateProcessList(process_list);
  process *cur = *process_list;

  while (cur != NULL)
  {
    printf("PID          Command          STATUS\n");
    printf("%d           %s               %d\n", cur->pid, (cur->cmd->arguments)[0], cur->status);
    if (cur->status != TERMINATED)
    {
      addProcess(&newList, cur->cmd, cur->pid);
    }
    else
    {
      freeCmdLines(cur->cmd);
    }
    cur = cur->next;
  }
  process *temp = *process_list;
  *process_list = newList;
  freeProcessList(temp);
}

void execute(cmdLine *pCmdLine)
{
  int input, output;
  // if (pipe2(pipefd, O_CLOEXEC) == -1)
  // {
  //   perror("pipe");
  //   exit(EXIT_FAILURE);
  // }
  // close(pipefd[0]);
  // close(pipefd[1]);
  // if (cmdl->inputRedirect != NULL)
  // {
  //   pipefd[0] = open(cmdl->inputRedirect, O_RDONLY);
  //   dup2(pipefd[0], STDIN_FILENO);
  // }
  // if (cmdl->outputRedirect != NULL)
  // {
  //   pipefd[1] = open(cmdl->outputRedirect, O_CREAT | O_WRONLY | O_TRUNC);
  //   dup2(pipefd[1], STDOUT_FILENO);
  // }
  if (pCmdLine->inputRedirect != NULL)
  {
    close(STDIN_FILENO);
    input = open(pCmdLine->inputRedirect, O_RDONLY);
    if (input == -1)
    {
      perror("input: open() error");
      _exit(1);
    }
    dup2(input, STDIN_FILENO);
  }
  if (pCmdLine->outputRedirect != NULL)
  {
    close(STDOUT_FILENO);
    output = open(pCmdLine->outputRedirect, O_CREAT | O_WRONLY | O_APPEND);
    if (output == -1)
    {
      perror("output: open() error");
      _exit(1);
    }
    dup2(output, STDOUT_FILENO);
  }

  const char *path = (pCmdLine->arguments)[0];
  if (execvp(path, pCmdLine->arguments) == -1)
  {
    perror("execvp() error");
    _exit(1);
  }
}

void executePiped(process **process_list, cmdLine *cmd1, cmdLine *cmd2, int debug)
{

  int in_child1 = 0;
  int in_child2 = 0;
  pid_t child1_pid;
  pid_t child2_pid;

  cmd1->next = NULL;

  // int input = STDIN_FILENO;
  // int output = STDOUT_FILENO;

  // 1.Create a pipe.
  int pipefd[2];
  if (pipe(pipefd) == -1)
  {
    perror("pipe");
    if (debug)
      fprintf(stderr, "(parent_process>exiting…)\n");
    exit(EXIT_FAILURE);
  }
  if (debug)
    fprintf(stderr, "(parent_process>forking…)\n");
  // 2.Fork a first child process (child1).
  child1_pid = fork();

  if (child1_pid == -1)
  {
    perror("fork");
    if (debug)
      fprintf(stderr, "(parent_process>exiting…)\n");
    exit(EXIT_FAILURE);
  }

  addProcess(process_list, cmd1, child1_pid);

  in_child1 = (child1_pid == 0);
  if (debug & !in_child1)
    fprintf(stderr, "(parent_process>created process with id: %d)\n", child1_pid);
  // 3.In the child1 process:
  if (in_child1)
  {

    // 1.Close the standard output.
    close(STDOUT_FILENO);
    // 2.Duplicate the write-end of the pipe using dup (see man).
    if (debug)
      fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe…)\n");
    dup2(pipefd[1], STDOUT_FILENO);
    // 3.Close the file descriptor that was duplicated.
    close(pipefd[1]);
    // 4.Execute command1.
    if (debug)
      fprintf(stderr, "(child1>going to execute cmd: …)\n");
    execute(cmd1);
  }
  else if (!in_child1 & !in_child2)
  {
    // 4.In the parent process: Close the write end of the pipe.
    if (debug)
      fprintf(stderr, "(parent_process>closing the write end of the pipe…)\n");
    close(pipefd[1]);
    // 5.Fork a second child process (child2).
    if (debug)
      fprintf(stderr, "(parent_process>forking…)\n");
    child2_pid = fork();
    if (child2_pid == -1)
    {
      perror("fork");
      if (debug)
        fprintf(stderr, "(parent_process>exiting…)\n");
      exit(EXIT_FAILURE);
    }

    addProcess(process_list, cmd2, child2_pid);

    in_child2 = (child2_pid == 0);
    if (debug)
      fprintf(stderr, "(parent_process>created process with id: %d)\n", child2_pid);
  }

  // 6.In the child2 process:
  if (in_child2)
  {
    // 1.Close the standard input.
    close(STDIN_FILENO);
    // 2.Duplicate the read-end of the pipe using dup.
    if (debug)
      fprintf(stderr, "(child2>redirecting stdin to the write end of the pipe…)\n");
    dup2(pipefd[0], STDIN_FILENO);
    // 3.Close the file descriptor that was duplicated.
    close(pipefd[0]);
    // 4.Execute command2.
    if (debug)
      fprintf(stderr, "(child2>going to execute cmd: …)\n");
    execute(cmd2);
  }
  // Both children are out, back to parent
  // 7.In the parent process: Close the read end of the pipe.
  if (debug)
    fprintf(stderr, "(parent_process>closing the read end of the pipe…)\n");
  close(pipefd[0]);

  // 8.Now wait for the child processes to terminate, in the same order of their execution.
  int status1;
  int status2;
  if (debug)
    fprintf(stderr, "(parent_process>waiting for child processes to terminate…)\n");
  pid_t w = waitpid(child1_pid, &status1, WUNTRACED | WCONTINUED);
  if (w == -1)
  {
    perror("waitpid");
    if (debug)
      fprintf(stderr, "(parent_process>exiting…)\n");
    exit(EXIT_FAILURE);
  }
  w = waitpid(child2_pid, &status2, WUNTRACED | WCONTINUED);
  if (w == -1)
  {
    perror("waitpid");
    if (debug)
      fprintf(stderr, "(parent_process>exiting…)\n");
    exit(EXIT_FAILURE);
  }
  if (debug)
    fprintf(stderr, "(parent_process>exiting…)\n");
}

void shiftLeft(char *array)
{
  char *arr = *array;
  free(arr);
  for (int i = 1; i < HISTLEN - 1; i++)
  {
    *(arr + i - 1) = *(arr + i);
  }
}

int main(int argc, char **argv)
{
  int debug = 0;
  int status;
  char path[PATH_MAX];
  char line[BUFFERLENGTH];
  // int pipefd[2];
  char *history[HISTLEN];
  int current_free = 0;
  char *copy;
  process *processList = NULL;
  for (int i = 1; i < argc; i++)
  {
    if ((strncmp(argv[i], "-d", 2) == 0))
    {
      debug = 1;
    }
  }
  if (getcwd(path, sizeof(path)) == NULL)
    perror("getcwd() error");

  while (1)
  {

    printf("%s> ", path);
    // current_free = current_free % HISTLEN;
    fgets(line, BUFFERLENGTH, stdin);
    // if (history[current_free] != NULL)
    // {
    //   free(history[current_free]);
    // }
    copy = malloc(sizeof(char *));
    strcpy(copy, line);

    if (current_free == HISTLEN)
    {
        char* temp = history[0];
        for (int i = 1; i < HISTLEN; i++)
        {
            history[i - 1] = history[i];
        }
        free(temp);
        current_free = HISTLEN - 1;
    }

    history[current_free] = copy;
    current_free++;


    printf("cur free is:%d\n", current_free);
    if (strncmp("cd ", line, 3) == 0)
    {

      char *p = &(line[3]);
      p[strlen(p) - 1] = '\0';
      int res = chdir(p);

      if (res == -1)
      {
        perror("chdir() error");
      }
      else
      {
        if (getcwd(path, sizeof(path)) == NULL)
          perror("getcwd() error");
      }
    }
    else if (strcmp("quit\n", line) == 0)
    {
      exit(0);
    }
    else if (strcmp("procs\n", line) == 0)
    {
      printProcessList(&processList);
    }
    else if (strncmp("suspend", line, 7) == 0)
    {
      pid_t p_id = atoi(line + 8);
      kill(p_id, SIGTSTP);
    }
    else if (strncmp("kill", line, 4) == 0)
    {
      pid_t p_id = atoi(line + 5);
      kill(p_id, SIGINT);
    }
    else if (strncmp("wake", line, 4) == 0)
    {
      pid_t p_id = atoi(line + 5);
      kill(p_id, SIGCONT);
    }
    else if (strcmp("history\n", line) == 0)
    {
      for (int i = 0; i < HISTLEN; i++)
      {
        if (history[i] != NULL)
        {
          printf("%d)%s", i, history[i]);
        }
      }
    }

    else
    {
        cmdLine *cmdl;
        if (strcmp("!!\n", line) == 0)
        {
            for (int i = current_free - 2 ; i >= 0; i--)
            {
                if((history[i] != NULL) && (strcmp(history[i], "history") != 0)){
                    cmdl = parseCmdLines(history[i]);
                    break;
                }
                if((history[i] != NULL) && (strcmp(history[i], "history") == 0) & (i == 0)){
                    printf("no non-history commands in the history");
                }
            }
        }
        else if(line[0] == '!')
        {
            int index = atoi(line + 1);
            if((index >= HISTLEN) | (index < 0)){
                printf("illegal command index");
            }
            else {
                cmdl = parseCmdLines(history[index]);
            }
        }
        else{
            cmdl = parseCmdLines(line);
        }

      if (cmdl->next != NULL)
      {
        executePiped(&processList, cmdl, cmdl->next, debug);
      }
      else
      {
        pid_t cpid = fork();
        if (cpid == -1)
        {
          perror("fork");
          _exit(EXIT_FAILURE);
        }

        int child = (cpid == 0);
        if (child)
        {
          if (debug)
          {
            fprintf(stderr, "Executing command: %s", line);
            fprintf(stderr, "PID: %d\n", getpid());
          }
          execute(cmdl);
          exit(1); /* error! */
        }
        if (!child)
        {
          addProcess(&processList, cmdl, cpid);
          if (cmdl->blocking != 0)
          {
            pid_t w = waitpid(cpid, &status, WUNTRACED | WCONTINUED);
            if (w == -1)
            {
              perror("waitpid");
              _exit(EXIT_FAILURE);
            }
          }
        }
      }
    }
  }

  return 0;
}