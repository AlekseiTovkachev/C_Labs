#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <elf.h>

#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2


Elf32_Ehdr *ehdr;
Elf32_Shdr *shdr;
Elf32_Phdr *phdr;
void *ptr;

int currentFid;

typedef struct {
    int debug_mode;
    char file_name[128];
} state;

void mapFile(state *s);

Elf32_Shdr *get_section(Elf32_Word idx);

struct fun_desc {
    char *name;

    void (*fun)(state *);
};


void quit(state *s) {
    if (s->debug_mode) {
        printf("Quitting...\n");
    }
    exit(0);
}

void toggleDebug(state *s) {
    if (s->debug_mode) {
        s->debug_mode = 0;
        printf("Debug flag now off\n");
    } else {
        s->debug_mode = 1;
        printf("Debug flag now on\n");
    }
}

Elf32_Shdr *getTable(char *table_name) {
    Elf32_Word sh_entsize = ehdr->e_shentsize;
    Elf32_Word sh_offset = ehdr->e_shoff;
    Elf32_Word shstrndx = ehdr->e_shstrndx;
    Elf32_Shdr *name_table = get_section(shstrndx);
    for (int i = 0; i < ehdr->e_shnum; i++) {
        Elf32_Shdr *temp_shdr = (Elf32_Shdr *) ((char *) ehdr + sh_offset + i * sh_entsize);
        char *name = (char *) ehdr + name_table->sh_offset + temp_shdr->sh_name;
        if (strcmp(name, table_name) == 0) {
            return temp_shdr;
        }
    }
    return NULL;
}

void relocationTables(state *s) {
    mapFile(s);
    Elf32_Shdr *_reltab = NULL;
    Elf32_Shdr *dynsym = getTable(".dynsym");
    Elf32_Shdr *dynstr = getTable(".dynstr");
    Elf32_Shdr *name_table = get_section(ehdr->e_shstrndx);
    for (int i = 0; i < ehdr->e_shnum; i++) {
        if ((shdr[i].sh_type == SHT_REL) || (shdr[i].sh_type == SHT_RELA)) {
            _reltab = &shdr[i];
            unsigned int size = shdr[i].sh_size / shdr[i].sh_entsize;
            char *section_name = (char *) ehdr + name_table->sh_offset + _reltab->sh_name;
            printf("Relocation section '%s' at offset %#x contains %d entries:\n", section_name, _reltab->sh_offset,
                   size);
            printf(" %-10.10s  %-10.10s  %-10s  %-10.10s  %-10.10s\n", "Offset", "Info", "Type", "Sym.Value",
                   "Sym.name");
            for (int j = 0; j < size; j++) {
                Elf32_Rel *entry = (Elf32_Rel *) ((int) ehdr + shdr[i].sh_offset + j * shdr[i].sh_entsize);
                int index = ELF32_R_SYM(entry->r_info);
                unsigned int symaddr = (int) ehdr + dynsym->sh_offset;
                Elf32_Sym *symbol = &((Elf32_Sym *) symaddr)[index];
                unsigned int straddr = (int) ehdr + dynstr->sh_offset;
                char *name = &((char *) (straddr))[symbol->st_name];
                printf(" %-10.10x  %-10.10x  %-10x  %-10.10x  %-10s\n", entry->r_offset, entry->r_info,
                       ELF32_R_TYPE(entry->r_info), symbol->st_value, name);
            }
        }
    }
}

void printSymbolTable(Elf32_Sym *symtab, char *strtab, unsigned int symtab_size) {
    Elf32_Word sh_entsize = ehdr->e_shentsize;
    Elf32_Word sh_offset = ehdr->e_shoff;
    Elf32_Word shstrndx = ehdr->e_shstrndx;
    Elf32_Shdr *name_table = get_section(shstrndx);
    printf(" [%2s]  %-10.10s  %-10.10s  %-10.10s  %-10.10s\n", "Nr", "Value", "Ndx", "S.name", "Name");
    for (int i = 0; i < symtab_size; i++) {
        if (ELF32_ST_TYPE(symtab[i].st_info) == STT_SECTION) {
            Elf32_Shdr *temp_shdr = (Elf32_Shdr *) ((char *) ehdr + sh_offset + symtab[i].st_shndx * sh_entsize);
            char *section_name = (char *) ehdr + name_table->sh_offset + temp_shdr->sh_name;
            printf(" [%2d]  %-10.10x  %-10.10x  %-10.10s  %-10.10s\n", i, symtab[i].st_value, symtab[i].st_shndx,
                   section_name, section_name);
        } else if (symtab[i].st_shndx == SHN_UNDEF) {
            printf(" [%2d]  %-10.10x  %-10.10s  %-10.10s  %-10.10s\n", i, symtab[i].st_value, "UND", "",
                   strtab + symtab[i].st_name);
        } else if (symtab[i].st_shndx == SHN_ABS) {
            printf(" [%2d]  %-10.10x  %-10.10s  %-10.10s  %-10.10s\n", i, symtab[i].st_value, "ABS", "",
                   strtab + symtab[i].st_name);
        } else {
            Elf32_Shdr *temp_shdr = (Elf32_Shdr *) ((char *) ehdr + sh_offset + symtab[i].st_shndx * sh_entsize);
            char *section_name = (char *) ehdr + name_table->sh_offset + temp_shdr->sh_name;
            printf(" [%2d]  %-10.10x  %-10.10x  %-10.10s  %-10.10s\n", i, symtab[i].st_value, symtab[i].st_shndx,
                   section_name, strtab + symtab[i].st_name);
        }
    }
}

void printSymbols(state *s) {
    mapFile(s);
    Elf32_Shdr *_symtab = NULL;
    for (int i = 0; i < ehdr->e_shnum; i++) {
        if (shdr[i].sh_type == SHT_SYMTAB) {
            _symtab = &shdr[i];
            Elf32_Sym *symtab = (Elf32_Sym *) (ptr + _symtab->sh_offset);
            Elf32_Shdr *_strtab = &shdr[_symtab->sh_link];
            char *strtab = (char *) (ptr + _strtab->sh_offset);
            unsigned int symtab_size = _symtab->sh_size / sizeof(Elf32_Sym);
            if (s->debug_mode) printf("Size: %d Number of symbols: %d\n", _symtab->sh_size, symtab_size);
            printSymbolTable(symtab, strtab, symtab_size);
        }
    }
    if (_symtab == NULL) {
        fprintf(stderr, "No symbol table to print");
        exit(1);
    }
}


Elf32_Shdr *get_section(Elf32_Word idx) {
    return (Elf32_Shdr *) ((char *) ehdr + ehdr->e_shoff + idx * ehdr->e_shentsize);
}

const char *get_section_type(unsigned int type) {
    switch (type) {
        case 0: return "NULL";
        case 1: return "PROGBITS";
        case 2: return "SYMTAB";
        case 3: return "STRTAB";
        case 4: return "RELA";
        case 5: return "HASH";
        case 6: return "DYNAMIC";
        case 7: return "NOTE";
        case 8: return "NOBITS";
        case 9: return "REL";
        case 10:return "SHLIB";
        case 11:return "DYNSYM";
        case 14:return "INIT_ARRAY";
        case 15:return "FINI_ARRAY";
        case 16:return "PREINIT_ARRAY";
        case 17:return "GROUP";
        case 18:return "SYMTAB_SHNDX";
        case 19:return "NUM";
        default:return "UNKNOWN";
    }
}

void printSectionNames(state *s) {
    mapFile(s);
    Elf32_Word sh_entsize = ehdr->e_shentsize;
    Elf32_Word sh_offset = ehdr->e_shoff;
    Elf32_Word shstrndx = ehdr->e_shstrndx;
    Elf32_Shdr *name_table = get_section(shstrndx);
    if (s->debug_mode) {
        printf("shstrnx: %x\n", shstrndx);
        printf("string table offset: %x\n", name_table->sh_offset);
    }
    printf(" [%2s]  %-10.10s  %-10.10s  %-10.10s  %-10.10s  %-10.10s \n", "Nr", "Name", "Addr", "Off", "Size", "Type");
    for (int i = 0; i < ehdr->e_shnum; i++) {
        Elf32_Shdr *temp_shdr = (Elf32_Shdr *) ((char *) ehdr + sh_offset + i * sh_entsize);
        char *name = (char *) ehdr + name_table->sh_offset + temp_shdr->sh_name;
        const char *type_name = get_section_type(temp_shdr->sh_type);
        printf(" [%2d]  %-10.10s  %-10.10x  %-10.10x  %-10.10x  %-10.10s \n", i, name, temp_shdr->sh_addr,
               temp_shdr->sh_offset, temp_shdr->sh_size, type_name);
    }
}

void printProgramHeaderSizes() {
    printf("Program header sizes:\n");
    for (int i = 0; i < ehdr->e_phnum; i++) {
        size_t size = phdr[i].p_filesz;
        printf("%#x\n", size);
    }
}

void printSectionHeaderSizes() {

    printf("Section header sizes:\n");
    for (int i = 0; i < ehdr->e_shnum; i++) {
        size_t size = shdr[i].sh_size;
        printf("%#x\n", size);
    }
}

char *getDataTypeLine(int dataType) {
    if (dataType == ELFDATANONE) {
        return "unknown";
    } else if (dataType == ELFDATA2LSB) {
        return "2's complement, little endian";
    } else if (dataType == ELFDATA2MSB) {
        return "2's complement, big endian";
    }
    return NULL;
}

int checkIfELF(void *e_ptr) {
    if ((((char *) e_ptr)[1] != 'E') || (((char *) e_ptr)[2] != 'L') || (((char *) e_ptr)[3] != 'F') ||
        (((char *) e_ptr)[0] != 0x7F)) {
        return 0;
    }
    return 1;
}


void examineELFFile(state *s) {

    printf("Please provide the file name...\n");
    scanf("%s", s->file_name);
    if (s->debug_mode) { printf("Debug: file name set to '%s'\n", s->file_name); }
    if (currentFid != 0) { close(currentFid); }
    currentFid = open(s->file_name, O_RDWR);
    if (currentFid == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    unsigned int size = lseek(currentFid, 0, SEEK_END);
    lseek(currentFid, 0, SEEK_SET);
    ptr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, currentFid, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    if (checkIfELF(ptr) == 0) {
        return;
    }
    ehdr = (Elf32_Ehdr *) ptr;
    shdr = (Elf32_Shdr *) (ptr + ehdr->e_shoff);
    phdr = (Elf32_Phdr *) (ptr + ehdr->e_phoff);
    //printf("first section name: %s\n", shdr->sh_name);

    printf("Magic: %c %c %c\n", ((char *) ptr)[1], ((char *) ptr)[2], ((char *) ptr)[3]);
    char *dataType = getDataTypeLine(((int *) ptr)[5]);
    printf("Data: %s\n", dataType);

    printf("Entry point address: %#x\n", ehdr->e_entry);
    printf("Section header table offset: %d\n", ehdr->e_shoff);
    printf("Number of section entries: %d\n", ehdr->e_shnum);
    printSectionHeaderSizes();

    printf("Program header table offset: %d\n", ehdr->e_phoff);
    printf("Number of program entries: %d\n", ehdr->e_phnum);
    printProgramHeaderSizes();

}

void mapFile(state *s) {

    printf("Please provide the file name...\n");
    scanf("%s", s->file_name);
    if (s->debug_mode) { printf("Debug: file name set to '%s'\n", s->file_name); }
    if (currentFid != 0) { close(currentFid); }
    currentFid = open(s->file_name, O_RDWR);
    if (currentFid == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    unsigned int size = lseek(currentFid, 0, SEEK_END);
    lseek(currentFid, 0, SEEK_SET);
    ptr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, currentFid, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    if (checkIfELF(ptr) == 0) {
        return;
    }
    ehdr = (Elf32_Ehdr *) ptr;
    shdr = (Elf32_Shdr *) (ptr + ehdr->e_shoff);
    phdr = (Elf32_Phdr *) (ptr + ehdr->e_phoff);
    close(currentFid);
}

void menu(state *s) {

    struct fun_desc menu[] = {{"0)  Toggle Debug Mode\n",   toggleDebug},
                              {"1)  Examine ELF File\n",    examineELFFile},
                              {"2)  Print Section Names\n", printSectionNames},
                              {"3)  Print Symbols\n",       printSymbols},
                              {"4)  Relocation Tables\n",   relocationTables},
                              {"5)  Quit\n",                quit},
                              {NULL, NULL}};
    while (1) {
        if (s->debug_mode) {

        }
        int i = 0;
        while (menu[i].name != NULL) {
            printf("%s", menu[i++].name);
        }
        printf("        Option : ");
        int c = fgetc(stdin);
        if (c == '\n') {
            c = fgetc(stdin);
        }
        printf("%c\n", c);
        if (('0' <= c) & (c <= '9')) {
            printf("Within bounds\n");
            int option = c - '0';
            menu[option].fun(s);
            printf("DONE\n");
        } else {
            printf("Not within bounds\n");
            exit(0);
        }
    }
}

int main(int argc, char **argv) {
    int debug = 0;
    for (int i = 0; i < argc; i++) {
        if ((strncmp(argv[i], "-d", 2) == 0)) {
            debug = 1;
        }
    }
    state *s = malloc(sizeof(state));
    s->debug_mode = debug;
    strcpy(s->file_name, "");
    s->file_name[0] = '\0';

    menu(s);
}