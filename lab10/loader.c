#include <elf.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

void foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg);
int checkIfELF(void *e_ptr);
void load_phdr(Elf32_Phdr *phdr, int fd);
int startup(int argc, char **argv, void (*start)());

void getFlagString(int flg, char *flags) {
  flags[0] = ' ';
  flags[1] = ' ';
  flags[2] = ' ';
  flags[3] = '\0';
  if (flg & PF_R) flags[0] = 'R';
  if (flg & PF_W) flags[1] = 'W';
  if (flg & PF_X) flags[2] = 'E';
}

char *getTypeString(int type) {
  switch (type) {
    case 0:
      return "NULL";
    case 1:
      return "LOAD";
    case 2:
      return "DYNAMIC";
    case 3:
      return "INTERP";
    case 4:
      return "NOTE";
    case 5:
      return "SHLIB";
    case 6:
      return "PHDR";
    default:
      return "UNKNOWN";
  }
}

int checkIfELF(void *e_ptr) {
  if ((((char *)e_ptr)[0] != ELFMAG0) || (((char *)e_ptr)[1] != ELFMAG1) ||
      (((char *)e_ptr)[2] != ELFMAG2) || (((char *)e_ptr)[3] != ELFMAG3)) {
    return 0;
  }
  return 1;
}

void foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg) {
  Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_start;
  for (int i = 0; i < ehdr->e_phnum; i++) {
    Elf32_Phdr *temp_phdr =
        (Elf32_Phdr *)((char *)ehdr + ehdr->e_phoff + i * ehdr->e_phentsize);
    func(temp_phdr, arg);
  }
}

void print_phdr_old(Elf32_Phdr *phdr, int arg) {
  printf("Program header number %d at address %#x\n", arg, phdr->p_vaddr);
}

void print_phdr(Elf32_Phdr *phdr, int arg) {
  char flags[4];
  getFlagString(phdr->p_flags, flags);
  printf(
      " %-8.8s  0x%-8.8x  0x%-8.8x  0x%-8.8x  0x%-8.8x  0x%-8.8x  %-3.3s  "
      "0x%-x\n",
      getTypeString(phdr->p_type), phdr->p_offset, phdr->p_vaddr, phdr->p_paddr,
      phdr->p_filesz, phdr->p_memsz, flags, phdr->p_align);
}

int getProtFlags(Elf32_Word flags) {
  int mFlags = PROT_NONE;
  printf("Flags: PROT_NONE");
  if (flags & PF_R){
    mFlags = mFlags | PROT_READ;
    printf("| PROT_READ");
  }
  if (flags & PF_W) {
    mFlags = mFlags | PROT_WRITE;
    printf("| PROT_WRITE");
  }
  if (flags & PF_X) {
    mFlags = mFlags | PROT_EXEC;
    printf("| PROT_EXEC");
  }
  printf("\n");
  return mFlags;
}

void load_phdr(Elf32_Phdr *phdr, int fd) {
  if (phdr->p_type == PT_LOAD) {
    int vaddr = phdr->p_vaddr & 0xfffff000;
    int offset = phdr->p_offset & 0xfffff000;
    int padding = phdr->p_vaddr & 0xfff;

    int prot = getProtFlags(phdr->p_flags);

    int mapping = MAP_PRIVATE | MAP_FIXED;
    printf("Mapping: MAP_PRIVATE | MAP_FIXED\n");
    void *ptr =
        mmap((void *)vaddr, phdr->p_memsz + padding, prot, mapping, fd, offset);
    if (ptr == MAP_FAILED) {
      perror("fucking mmap failed");
      return;
    }
    print_phdr(phdr, 0);
  }
}

int main(int argc, char **argv) {
  int fd = open(argv[1], O_RDWR);
  unsigned int size = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  Elf32_Ehdr *ehdr = (Elf32_Ehdr *)ptr;
  if (ptr == MAP_FAILED) {
    perror("mmap1");
    exit(EXIT_FAILURE);
  }
  if (checkIfELF(ptr) == 0) {
    fprintf(stderr, "Invalid file");
    return 1;
  }
  //foreach_phdr(ptr, print_phdr, fd);
  foreach_phdr(ptr, load_phdr, fd);
  startup(argc - 1, argv + 1, (void *)(ehdr->e_entry));
  return 0;
}