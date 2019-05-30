#include "../include/utils.hpp"
#include <string.h>
#include <stdio.h>
#include <vector>

int intFromString(char str[]){
    int result = 0;

    for(int i = 0; i < strlen(str); i++)
        result = result * 10 + (str[i] - '0');

    return result;
}

int readIntLine(FILE *fp){
    char c, numRead[20];
    int i = 0;

    c = fgetc(fp);
    while(c != '\n'){
        numRead[i] = c;
        c = fgetc(fp);
        i++;
    }

    numRead[i] = '\0';
    return intFromString(numRead);
}

bool loadFile(int *numberOfTasks,
              int *numberOfMachines,
              int *deadline,
              int **start,
              int **duration,
              int **machine,
              char *filename){

    FILE *fp = fopen(filename, "r");

    if(!fp) return false;

    *numberOfMachines = readIntLine(fp);
    *numberOfTasks = readIntLine(fp);
    *deadline = readIntLine(fp);

    (*start) = new int[*numberOfTasks];
    (*duration) = new int[*numberOfTasks];
    (*machine) = new int[*numberOfTasks];

    for(int i = 0; i < *numberOfTasks; i++){
        (*start)[i] = 0;
        (*machine)[i] = 0;
        (*duration)[i] = readIntLine(fp);
    }

    fclose(fp);
    return true;
}

int vmax(int a, int b){
    return (a > b) ? a : b;
}