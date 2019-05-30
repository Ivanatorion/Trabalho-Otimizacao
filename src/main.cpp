#include "../include/simulatedAnnealing.hpp"
#include "../include/utils.hpp"
#include <string.h>
#include <errno.h>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <math.h>
#include <ctime>

int *freeTime;
int *randomTasks;
int *randomMachines;

int main(int argc, char* argv[]){
    // Input
    int numberOfTasks, numberOfMachines, deadline;
    int initialValue, goodInitialValue;
    int* duration;

    // Output
    int* start;
    int* machine;

    printf("%d parametros encontrados.\n", argc);
    if(argc != 3){
        printf("Parametros da linha de comando: ARQUIVO_DE_ENTRADA SEED\n");
        return 0;
    }

    int seed = intFromString(argv[2]);
    if(seed == 0){
        seed = time(0);
    }

    srand(seed);

    // Read input file
    printf("Carregando arquivo %s... ", argv[1]);
    if(!loadFile(&numberOfTasks, &numberOfMachines, &deadline, &start, &duration, &machine, argv[1])){
        printf("Nao pode abrir %s\n", argv[1]);
        return 0;
    }
    printf("Pronto!\n");

    freeTime = new int[numberOfMachines];
    randomTasks = new int[numberOfTasks];
    randomMachines = new int[numberOfMachines];

    printf("Inicializando estruturas randomizadas... ");
    #pragma omp parallel for num_threads(4)
    for(int i = 0; i < numberOfMachines; i++)
        randomMachines[i] = i;

    #pragma omp parallel for num_threads(4)
    for(int i = 0; i < numberOfTasks; i++)
        randomTasks[i] = i;
    printf("Pronto!\n");

    // Cria a solução inicial
    printf("Gerando soluções iniciais... ");
    createInitialSolutionG(numberOfTasks, numberOfMachines, deadline, start, duration, machine);
    goodInitialValue = valueOfSolution(numberOfTasks, numberOfMachines, deadline, start, duration, machine);

    createInitialSolution(numberOfTasks, numberOfMachines, deadline, start, duration, machine);
    initialValue = valueOfSolution(numberOfTasks, numberOfMachines, deadline, start, duration, machine);
    printf("Pronto!\n");

    printf("Seed utilizada: %d\n", seed);
    printf("Criada solucao inicial com valor: %d\n", initialValue);

    if(isValid(numberOfTasks, numberOfMachines, deadline, start, duration, machine)) {
        printf("A solucao inicial eh valida\n");
    } else {
        printf("ERRO: Solucao inicial invalida!\n");
        return 0;
    }

    //Arquivo CSV
    char filename[200];
    strcpy(filename, argv[1]);
    strcpy(strstr(filename, "."), "Solution.csv");

    //Roda o simulated annealing
    int temperature = vmax(10, initialValue / 2750);
    int numberOfNeighbors = vmax(numberOfTasks * 25, 50);
    simulatedAnnealing(numberOfTasks,
                       numberOfMachines,
                       deadline,
                       start,
                       duration,
                       machine,
                       temperature,
                       0.9,
                       numberOfNeighbors,
                       filename);

    //Informa o valor da nova solucao
    printf("Valor inicial bom: %d\n", goodInitialValue);
    printf("Valor inicial: %d\n", initialValue);
    printf("Novo Valor: %d\n", valueOfSolution(numberOfTasks, numberOfMachines, deadline, start, duration, machine));

    if(isValid(numberOfTasks, numberOfMachines, deadline, start, duration, machine)) {
        printf("A solucao final eh valida\n");
    } else {
        printf("ERRO: Solucao final invalida!\n");
        return 0;
    }

    //Salva a solucao em um arquivo
    strcpy(filename, argv[1]);
    strcpy(strstr(filename, "."), "Solution.txt");
    saveSolution(numberOfTasks, numberOfMachines, deadline, start, duration, machine, filename);

    printf("\nResultado salvo em: %s\n", filename);

    delete[] freeTime;
    delete[] randomMachines;
    delete[] randomTasks;
    delete[] start;
    delete[] duration;
    delete[] machine;

    return 0;
}
