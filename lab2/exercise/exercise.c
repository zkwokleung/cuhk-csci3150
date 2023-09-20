#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*

Under the same folder, file input.txt.
Each line contains a positive integer number and we guarantee the number is smaller than 1000.
You need to read the number in each line, add it by one, and write them line by line to another file called output.txt

NOTE: Please don't hardcode the results in your program because we change the content in input.txt when grading.

TIP: You can use sscanf to convert char array to int and sprintf to convert int to char array. 

*/
int main(){
    int fdIn;
    int fdOut;
    if ((fdIn = open("input.txt", O_RDONLY)) < 0)
    {
        printf("Error openning input.txt\n");
        exit(-1);
    }
    if ((fdOut = open("output.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR)) < 0)
    {
        printf("Error openning output.txt\n");
        exit(-1);
    }

    char inBuf[40];
    char outBuf[5];
    int num;
    int sum = 0;
    if(read(fdIn, inBuf, 40) < 0)
    {
        printf("Error reading input.txt\n");
        exit(-1);
    }

    char *token = strtok(inBuf, "\n");
    while(token != NULL)
    {
        // Convert to number
        sscanf(token, "%d", &num);
        // Sum up the numbers
        sum = num + 1;
        // Convert back to char array
        sprintf(outBuf, "%d\n", sum);
        // Write to output.txt
        if(write(fdOut, outBuf, strlen(outBuf)) < 0)
        {
            printf("Error writing output.txt\n");
            exit(-1);
        }
        // next token
        token = strtok(NULL, "\n");
    }

    close(fdIn);
    close(fdOut);
    
    return 0;
}
