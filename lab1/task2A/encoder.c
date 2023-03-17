#include <stdio.h>
#include <string.h>

int a_letter = 97;
int z_letter = 122;
int A_letter = 65;
int Z_letter = 122;
int to_upper = -32;
//int numeric_char_to_int = -48;
/*
 * On this lab you can assume that the resulting encrypted char will always be in the range from 32 to 126 (inclusive).
 * Possible arguments:
 * -D
 * +e123
 * -e123
 * -iFILE
 * -oOUTPUT
 *  ERRORS:
 * -If FILE cannot be opened for reading, print an error message and return 0;
 */
int main(int argc, char **argv) {
    FILE* output_stream = stdout;
    FILE* input_stream = stdin;
    char* key = "-e0";
    int debug_mode = 0;
    int encoding = 0;
    int ending_index = 0;
    int current_index = -1;
    int adding = 0;
    for (int i = 1; i < argc; i++){
        if(strcmp(argv[i],"-D") == 0){
            printf("%s\n", argv[i]);
            debug_mode = 1;
        }
        else if((strlen(argv[i]) > 2) && ((argv[i][0] == '+')) & (argv[i][1] == 'e')){
            key = &argv[i][2];
            ending_index = strlen(key) - 1;
            encoding = 1;
            adding = 1;
        }
        else if((strlen(argv[i]) > 2) && ((argv[i][0] == '-')) & (argv[i][1] == 'e')){
            key = &argv[i][2];
            ending_index = strlen(key) - 1;
            encoding = 1;
        }
        else if((strlen(argv[i]) > 2) && ((argv[i][0] == '-')) & (argv[i][1] == 'i')){
            input_stream = fopen(&argv[i][2], "r");
            if(input_stream == NULL){
                fprintf(stderr, "ERROR: error while opening file\n");
                return 1;
            }
        }
        else if((strlen(argv[i]) > 2) && ((argv[i][0] == '-')) & (argv[i][1] == 'o')){
            output_stream = fopen(&argv[i][2], "w");
        }
        //printf("%s\n", argv[i]);
    }
    int c = fgetc(input_stream);
    while(c != EOF){
        int encoded_c = c;
        int c_is_legal = (c >= 32) & (c <= 126);
        int c_is_lowercase = (c>=a_letter) & (c<=z_letter) & c_is_legal;
        if(encoding & c_is_legal){
            int to_add = key[++current_index] -'0';
            if(current_index == ending_index){
                current_index = -1;
            }
            if(adding){
                encoded_c += to_add;
            }
            else{
                encoded_c -= to_add;
            }
        }
        if((!encoding) & c_is_lowercase){
            encoded_c += to_upper;
        }
        if(debug_mode & c_is_legal){
            fprintf(stderr, "%X ", c);
        }
        if(debug_mode & c_is_legal){
            fprintf(stderr, "%X\n", encoded_c);
        }
        if(debug_mode & (c=='\n')){
            fputc('\n', stderr);
        }
        if(c =='\n'){
            current_index = -1;
            fputc(c, output_stream);
            c = fgetc(input_stream);
        }
        if(c_is_legal){
            fputc(encoded_c, output_stream);
            c = fgetc(input_stream);
        }
    }
    return 0;
}