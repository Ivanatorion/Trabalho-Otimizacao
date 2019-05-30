#ifndef TRABALHO_OTIMIZACAO_UTILS_HPP
#define TRABALHO_OTIMIZACAO_UTILS_HPP

#include <stdio.h>

int intFromString(char str[]);

int readIntLine(FILE *fp);

bool loadFile(int *numberOfTasks,
              int *numberOfMachines,
              int *deadline,
              int **start,
              int **duration,
              int **machine,
              char *filename);


int vmax(int a, int b);

#endif //TRABALHO_OTIMIZACAO_UTILS_HPP
