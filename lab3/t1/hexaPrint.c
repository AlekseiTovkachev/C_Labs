#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int printHex(FILE* buffer, int length){
    char *numbers = (char *)calloc(length, sizeof(char));
    fread(numbers, 1, length, buffer);
    for (int i = 0; i < length; i++)
    {
        printf("%02hhX ", numbers[i]);
    }
    free(numbers);
    printf("\n");
    return 0;
}









int main(int argc, char **argv)
{
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
    printHex(input_stream, length);   
    fclose(input_stream);
    
    return 0; 
}