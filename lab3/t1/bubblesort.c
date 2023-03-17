#include <stdio.h>
#include <stdlib.h>

void bubbleSort(int numbers[], int array_size)
{
    int i, j;
    int *temp;
    for (i = (array_size - 1); i > 0; i--)
    {
        for (j = 1; j <= i; j++)
        {
            //for(int k = 0; k < array_size; k++){
            //    printf("%d ", numbers[k]);
            //}
            //printf("\n");
            if (numbers[j - 1] > numbers[j])
            {
                temp = (int *)malloc(sizeof(int *));
                *temp = numbers[j - 1];
                numbers[j - 1] = numbers[j];
                numbers[j] = *temp;
                free(temp);
            }
        }
    }
    //free(temp);
}

int main(int argc, char **argv)
{  
    char **arr = argv + 1;
    int i, n = argc - 1;
    int *numbers = (int *)calloc(n, sizeof(int));
    if(argc == 1){
        //free(arr);
        free(numbers);
        return 0;
    }
    printf("Original array:");
    for (i = 0; i < n; ++i)
    {
        printf(" %s", arr[i]);
        numbers[i] = atoi(arr[i]);
    }
    
    printf("\n");

    bubbleSort(numbers, n);

    printf("Sorted array:");
    
    for (i = 0; i < n; ++i)
        printf(" %d", numbers[i]);

    printf("\n");

    //free(arr);
    free(numbers);
    return 0;
}