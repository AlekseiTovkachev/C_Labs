#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 
char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}
/* Ignores c, reads and returns a
 character from stdin using fgetc. */
char my_get(char c){
    return fgetc(stdin);
}

/* If c is a number between 0x20 and 0x7E, cprt prints the
 * character of ASCII value c followed by a new line.
 * Otherwise, cprt prints the dot ('.') character.
 * After printing, cprt returns the value of c unchanged. */
char cprt(char c){
    if((c > 0x20) & (c < 0x7E)){
        printf("%c\n", c);
    }
    else{
        printf(".\n");
    }
    return c;
}

/* Gets a char c and returns its encrypted form by adding 3 to its value.
 * If c is not between 0x20 and 0x7E it is returned unchanged */
char encrypt(char c){
    if((c > 0x20) & (c < 0x7E)){
        return (int)c + 3;
    }
    else{
        return c;
    }
}

/* Gets a char c and returns its decrypted form by
   reducing 3 to its value.
 * If c is not between 0x20 and 0x7E it is returned unchanged */
char decrypt(char c){
    if((c > 0x20) & (c < 0x7E)){
        return (int)c - 3;
    }
    else{
        return c;
    }
}

/* xprt prints the value of c in a hexadecimal
 representation followed by a new line,
 * and returns c unchanged. */
char xprt(char c){
    if(c != '\n'){
       printf("%X\n", c); 
    }
    else{
       printf(".\n"); 
    }
    return c;
}



/* Gets a char c, and if the char is 'q' ,
 ends the program with exit code 0. Otherwise, returns c. */
char quit(char c){
    if(c == 'q'){
        exit(0);
    }
    else{
        return c;
    }
}

char* map(char *array, int array_length, char (*f) (char)){
    char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
    for (int i = 0; i < array_length; ++i) {
        if(f == quit && array[i] == 'q'){
            free(mapped_array);
            free(array);
            f('q');
        }
        mapped_array[i] = f(array[i]);
    }
    return mapped_array;
}

struct fun_desc {
    char *name;
    char (*fun)(char);
};

void menu(){
    int base_len = 5;
    char arr[base_len];
    arr[0] = '\0';
    char* carray = arr;
    struct fun_desc menu[] = {
            { "0)  Get string\n", my_get },
            { "1)  Print string\n", cprt},
            { "2)  Print hex\n", xprt },
            { "3)  Censor\n", censor },
            { "4)  Encrypt\n", encrypt },
            { "5)  Decrypt\n", decrypt },
            { "6)  Quit\n", quit },
            { NULL, NULL }};
    while(1){
        int i = 0;
        while (menu[i].name != NULL){
            printf("%s", menu[i].name);
            i++;
        }
        printf("        Option : ");
        char option = fgetc(stdin);
        char bin = 'b';
        while(bin != '\n'){
            bin = fgetc(stdin);
        }
        printf("%c\n", option);

        if(('0'<= option) & (option <= '9')){
            printf("Within bounds\n");
            carray = map(carray, base_len, menu[option - '0'].fun);
            printf("DONE.\n");
        }
        else{
            printf("Not within bounds\n");
            free(carray);
            exit(0);
        }
    }
    }


int main(int argc, char **argv){
    //char arr1[] = {'H','E','Y','!'};
    //char* arr2 = map(arr1, 4, censor);
    //printf("%s\n", arr2);
    //free(arr2);

    /*int base_len = 5;
    char arr1[base_len];    
    
    char* arr2 = map(arr1, base_len, my_get);
    //char* arr3 = map(arr2, base_len, cprt);
    map(arr2, base_len, quit);
    */
    /*
    char* arr4 = map(arr3, base_len, xprt);
    char* arr5 = map(arr4, base_len, encrypt);
    char* arr6 = map(arr5, base_len, decrypt);
    free(arr2);
    free(arr3);
    free(arr4);
    free(arr5);
    free(arr6);
    */

    menu();

    return 0;


}

