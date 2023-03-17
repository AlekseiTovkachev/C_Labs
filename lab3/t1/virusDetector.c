#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct fun_desc {
    char *name;
    char (*fun)(char);
};

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus; 


typedef struct link {
    struct link *nextVirus;
    virus *vir;
} link;

virus* readVirus(FILE* input){
    char b0 = fgetc(input);
    char b1 = fgetc(input);

    short size = b0 | b1 << 8;

    virus* v = (virus*)malloc(size + 16 + sizeof(char *));
    //(*v).SigSize = fgetc(input);
    //short number = b1 | b0 << 8;
    (*v).SigSize = size;
    fread((*v).virusName, 1, 16, input);
    unsigned char* sig = (unsigned char*)malloc(size);
    fread(sig, 1, (*v).SigSize, input); 
    (*v).sig = sig;
    return v;
}

void printVirus(virus* virus, FILE* output){
    fprintf(output, "Name: %s\nSignature length: %d\nSignature:\n"
    , (*virus).virusName, (*virus).SigSize);
    for (int i = 0; i < (*virus).SigSize; i++)
    {
        fprintf(output,"%02hhX ", (*virus).sig[i]);
    }
    printf("\n");
}

void list_print(link *virus_list, FILE* output){
    link* current = virus_list;
    while(current != NULL){
        printVirus(current->vir, output);
        printf("\n");
        current = current->nextVirus;
    }
}
//adding to the end
/*link* list_append(link* virus_list, virus* data){
    link* newLink = (link *)malloc(sizeof(link));
    link* last = virus_list;
    while(virus_list->nextVirus != NULL){
        last = virus_list->nextVirus;
    }
    last->nextVirus = newLink;
    newLink->vir = data;
    return newLink;
}*/

//adding to the beginning
link* list_append(link* virus_list, virus* data){
    link* newLink = (link *)malloc(sizeof(link));
    newLink->vir = data;
    newLink->nextVirus = virus_list;
    return newLink;
}

void list_free(link *virus_list){
    link* current = virus_list;
    while (current != NULL)
    {
        link* temp = current;
        current = current->nextVirus;
        free(temp->vir->sig);
        free(temp->vir);
        free(temp);
    }
}

link* load_signatures(FILE* input){

    link* head = (link *)malloc(sizeof(link));
    link* currentLink = head;

    fseek(input, 0, SEEK_END);
    int length = ftell(input);
    rewind(input);
    fseek(input, 4, SEEK_CUR);

    int currentByte = 4;
    //setting up head
    if(currentByte < length){
        virus* v = readVirus(input);
        currentLink->vir = v;
        currentByte = ftell(input);
    }
    while(currentByte < length){
        virus* v = readVirus(input);
        currentLink = list_append(currentLink, v);
        currentByte = ftell(input);
    }
    head = currentLink;
    return head;
}



void detect_virus(char *buffer, unsigned int size, link *virus_list){

    link* currentLink = virus_list;
    short max_length = 0;

    while(currentLink != NULL){
        if(currentLink->vir->SigSize > max_length){
            max_length = currentLink->vir->SigSize;
        }
        currentLink = currentLink->nextVirus;
    }

    for (unsigned int start = 0; start < size; start++)
    {
        for (unsigned int end = start; (end < size) && ((end - start) < max_length); end++)
        {
        
            int length = end - start;
            //char part[length];
            //strncpy (part, buffer + sizeof(char)*start, length);
            link* currLink = virus_list;

            while(currLink != NULL){
                //char compare[length];
                //strncpy(compare, (const char *)(currLink->vir->sig), length);
                if ((currLink->vir->SigSize == length) && (memcmp((const char *)(buffer+start), (const char *)(currLink->vir->sig), length) == 0))
                {

                    printf("Found virus!\nStart: %d\nName: %s,SigSize: %d\n", start, currLink->vir->virusName, currLink->vir->SigSize);
                }
                currLink = currLink->nextVirus;
            }               
        }
    }
    
    
}

void kill_virus(char *fileName, int signitureOffset, int signatureSize)
{
    FILE *file = fopen(fileName, "r+");
    fseek(file, signitureOffset-18, SEEK_CUR);
    for (int i = 0; i < signatureSize+18; i++)
    {
        fwrite(0x90, 1, 1, file);
    }
    fclose(file);
}

int menu(){
    FILE* input_stream = NULL;
    //FILE* output;
    int menu_length = 7;
    link* head;
    char* file_name = "signatures-L";
    char* menu[] = {
             "0)  Placeholder \n",
             "1)  Load signatures \n",
             "2)  Print signatures \n",
             "3)  Detect viruses \n",           
             "4)  Fix file  \n",           
             "5)  Quit \n",           
             NULL }; 
    while(1){
        for (int i = 1; i < menu_length-1; i++)
        {
            printf("%s", menu[i]);
        }
        printf("        Option : ");
        char c = fgetc(stdin);
        if(c == '\n'){
            c = fgetc(stdin);
        }
        printf("%c\n", c);
        if(('1'<= c) & (c <= '5')){
            printf("Within bounds\n");
            int option = c - '0';
            if(option == 1){
                input_stream = fopen(file_name, "r");
                if(input_stream == NULL){
                    fprintf(stderr, "ERROR: error while opening file\n");
                    return 1;
                }
                head = load_signatures(input_stream);
            }
            else if(option == 2){
                list_print(head, stdout);                
            }
            else if(option == 3){
                char buffer[10000];
                FILE* toCheck = fopen("infected", "r");
                fread(buffer, 1, 10000, toCheck); 
                detect_virus(buffer, 10000, head);
                fclose(toCheck);
            }
            else if(option == 4){
                printf("Insert starting byte: ");
                int startingByte;
                scanf("%d", &startingByte);

                printf("Insert filename: ");
                char filename[20];
                scanf("%s", filename);

                printf("Insert size: ");
                int size;
                scanf("%d", &size);
                
                kill_virus(filename, startingByte, size);
               
                
                //printf("Not inplemented\n");
            }
            else if(option == 5){
                list_free(head);
                if(input_stream!=NULL){
                fclose(input_stream);
            }
                exit(0);
            }
            printf("DONE\n");
        }
        else{
            list_free(head);
            if(input_stream!=NULL){
                fclose(input_stream);
            }
            printf("Not within bounds\n");
            exit(0);
        }
    }
    if(input_stream!=NULL){
        fclose(input_stream);
    }
    return 0;
    }



        int main(int argc, char **argv)
    {    
        /*
        FILE* input_stream = stdin;
        for (int i = 1; i < argc; i++){
            input_stream = fopen(&argv[i][0], "r");
            if(input_stream == NULL){
                fprintf(stderr, "ERROR: error while opening file\n");
                return 1;
            }
            //printf("got the file\n");
        }
        fseek(input_stream, 0, SEEK_END);
        int length = ftell(input_stream);
        rewind(input_stream);
        fseek(input_stream, 4, SEEK_CUR);
        int currentByte = 4;
        while(currentByte < length){
            virus* v = readVirus(input_stream);
            printVirus(v, stdout);
            currentByte = ftell(input_stream);
            free(v->sig);
            free(v);
        }
        fclose(input_stream);
        */
        menu();

        return 0; 
}

