#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define INT sizeof(int)
#define SHORT sizeof(short)
#define BYTE sizeof(char)


typedef struct {
    char debug_mode;
    int display_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
} state;


struct fun_desc {
    char *name;

    void (*fun)(state *);
};


void loadIntoMemory2(state *s, int location, int length);

char *unit_to_format_hex(int unit) {
    static char *formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
    return formats[unit - 1];
}

char *unit_to_format_dec(int unit) {
    static char *formats[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};
    return formats[unit - 1];
}

/* Prints the buffer to screen by converting it to text with printf */
void print_units(FILE *output, char *buffer, int count, int unit_size, int displayMode) {
    char *end = buffer + unit_size * count;
    if (displayMode == 0)
    {
        fprintf(output, "Decimal\n=======\n");
    }
    else
    {
        fprintf(output, "Hexadecimal\n=======\n");
    }
    while (buffer < end) {
        //print ints
        int var = *((int *) (buffer));
        if (displayMode == 0) {
            fprintf(output, unit_to_format_dec(unit_size), var);
        } else {
            fprintf(output, unit_to_format_hex(unit_size), var);
        }
        buffer += unit_size;
    }
}

/* Writes buffer to file without converting it to text with write */
void write_units(FILE *output, char *buffer, int length, int unit_size) {
    fwrite(buffer, unit_size, length, output);
}

void quit(state *s) {
    if (s->debug_mode) {
        fprintf(stderr, "Quitting...\n");
    }
    exit(0);
}

void toggleDebug(state *s) {
    if (s->debug_mode) {
        s->debug_mode = 0;
        fprintf(stderr, "Debug flag now off\n");
    } else {
        s->debug_mode = 1;
        fprintf(stderr, "Debug flag now on\n");
    }
}

void toggleDisplay(state *s) {
    if (s->display_mode) {
        s->display_mode = 0;
        printf("Display flag now off, decimal representation\n");
    } else {
        s->display_mode = 1;
        printf("Display flag now on, hexadecimal representation\n");
    }
}

void printStateDebug(state *s) {
    fprintf(stderr, "State unit size: %d \n", s->unit_size);
    fprintf(stderr, "State filename: %s \n", s->file_name);
    fprintf(stderr, "State mem count: %d \n", s->mem_count);
}

void setFileName(state *s) {
    printf("Please provide the file name...\n");
    //fgets(s->file_name, sizeof(s->file_name), stdin);
    scanf("%s", s->file_name);
    if (s->debug_mode) {
        fprintf(stderr, "Debug: file name set to '%s'\n", s->file_name);
    }
}

void setUnitSize(state *s) {
    printf("Please provide the new size...\n");
    int size;
    scanf("%d", &size);
    if ((size == 1) | (size == 2) | (size == 4)) {
        s->unit_size = size;
        if (s->debug_mode) {
            fprintf(stderr, "Debug: set size to %d\n", size);
        }
    } else {
        fprintf(stderr, "illegal size\n");
    }
}

void saveIntoFile(state *s) {
    printf("Please enter <source-address> <target-location> <length>\n");

    fgetc(stdin);
    char buf[50];
    fgets(buf, 50, stdin);
    int sourceAddress, targetLocation, length;
    sscanf(buf, "%x %x %d", &sourceAddress, &targetLocation, &length);

    //check if file_name is empty
    if (strcmp(s->file_name, "") == 0) {
        fprintf(stderr, "File name is empty\n");
    }
    // open file_name for reading
    FILE *file = fopen(s->file_name, "r+");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file\n");
        return;
    }
    fseek(file, 0, SEEK_END);
    unsigned int size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (targetLocation > size) {
        fprintf(stderr, "Invalid target location\n");
        return;
    }
    fseek(file, targetLocation, SEEK_SET);
    if (sourceAddress == 0) {
        write_units(file, (char *) &s->mem_buf, length, s->unit_size);
    } else {
        write_units(file, (char *) sourceAddress, length, s->unit_size);
    }

    fclose(file);
}

void memoryModify(state *s) {
    printf("Please enter <location> <value>\n");
    fgetc(stdin);
    char buf[50];
    fgets(buf, 50, stdin);
    int location, value;
    sscanf(buf, "%x %x", &location, &value);

    if (s->debug_mode) {
        fprintf(stderr, "Location: %x\nVal: %x", location, value);
    }
    if (s->unit_size == INT) {
        s->mem_buf[location + 3] = (value >> 24) & 0xFF;
        s->mem_buf[location + 2] = (value >> 16) & 0xFF;
        s->mem_buf[location + 1] = (value >> 8) & 0xFF;
        s->mem_buf[location] = value & 0xFF;
    } else if (s->unit_size == SHORT) {
        s->mem_buf[location + 1] = (value >> 8) & 0xFF;
        s->mem_buf[location] = value & 0xFF;
    } else {
        s->mem_buf[location] = value;
    }

}

void loadIntoMemory(state *s) {
    printf("Please enter <location> <length>\n");

    fgetc(stdin);

    char locLen[10];
    fgets(locLen, 10, stdin);
    int location, length;
    sscanf(locLen, "%x %d", &location, &length);

    loadIntoMemory2(s, location, length);
}

void loadIntoMemory2(state *s, int location, int length) {
    // check if file_name is empty
    if (strcmp(s->file_name, "") == 0) {
        fprintf(stderr, "File name is empty\n");
    }
    // open file_name for reading
    FILE *file_to_open = fopen(s->file_name, "a+");
    if (file_to_open == NULL) {
        fprintf(stderr, "Failed to open file\n");
    }
    fseek(file_to_open, location, 0);
    s->mem_count = length * s->unit_size;
    fread(s->mem_buf, s->unit_size, length, file_to_open);
    fclose(file_to_open);
    if (s->debug_mode) {
        fprintf(stderr, "Filename: %s \nLocation: %X\nLength: %d\n", s->file_name, location, length);
    }
}


void memoryDisplay(state *s) {
    printf("Please enter <address> <length>\n");
    fgetc(stdin);
    char locLen[10];
    fgets(locLen, 10, stdin);
    int address, length;
    sscanf(locLen, "%x %d", &address, &length);
    print_units(stdout, (char *) &(s->mem_buf[address]), length, s->unit_size, s->display_mode);
}

void menu(state *s) {

    struct fun_desc menu[] = {{"0)  Toggle Debug Mode\n",   toggleDebug},
                              {"1)  Set File Name\n",       setFileName},
                              {"2)  Set Unit Size\n",       setUnitSize},
                              {"3)  Load Into Memory\n",    loadIntoMemory},
                              {"4)  Toggle Display Mode\n", toggleDisplay},
                              {"5)  Memory Display\n",      memoryDisplay},
                              {"6)  Save Into File\n",      saveIntoFile},
                              {"7)  Memory Modify\n",       memoryModify},
                              {"8)  Quit\n",                quit},
                              {NULL, NULL}};
    while (1) {
        if (s->debug_mode) {
            printStateDebug(s);
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
    s->file_name[0] = '\0';
    s->unit_size = 1;
    s->mem_count = 0;


    menu(s);
}
