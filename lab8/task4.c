#include <stdio.h>

int digit_cnt(char *string) {
    int counter = 0;
    int i = 0;
    while (string[i] != '\0') {
        if ((string[i] >= '0') & (string[i] <= '9')) {
            counter++;
        }
        i++;
    }
    return counter;
}

int main(int argc, char **argv) {
    printf("%d", digit_cnt(argv[1]));
}

