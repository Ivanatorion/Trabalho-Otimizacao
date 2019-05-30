#include "../include/simulatedAnnealing.hpp"
#include "../include/utils.hpp"
#include <algorithm>
#include <string.h>
#include <cstdlib>
#include <errno.h>
#include <cstdio>
#include <vector>
#include <math.h>
#include <ctime>
#include <omp.h>

#define EULER 2.7182819

using namespace std;

extern int *freeTime;
extern int *randomTasks;
extern int *randomMachines;

void sorts(int index[], int duration[], int numberOfTasks) {
    sort(index, (index + numberOfTasks - 1), [duration](int a, int b) {return duration[a] < duration[b];});
    sort(duration, (duration + numberOfTasks - 1), [](int a, int b) {return a < b;});
}

bool createInitialSolution(int numberOfTasks,
                           int numberOfMachines,
                           int deadline,
                           int start[],
                           int duration[],
                           int machine[]) {

    int i, j, process, chosenMachine;

    #pragma omp parallel for num_threads(4)
    for(j = 0; j < numberOfMachines; j++)
        freeTime[j] = deadline;

    int order[numberOfTasks];
    #pragma omp parallel for num_threads(4)
    for(i = 0; i < numberOfTasks; i++)
        order[i] = i;

    sorts(order, duration, numberOfTasks);

    for(i = 0; i < numberOfTasks; i++){
        chosenMachine = 0;
        process = order[i];

        while(freeTime[chosenMachine] - duration[process] < 0 && chosenMachine < numberOfMachines)
            chosenMachine++;

        chosenMachine = chosenMachine % numberOfMachines;

        start[process] = freeTime[chosenMachine] - duration[process];
        machine[process] = chosenMachine;
        freeTime[chosenMachine] -= duration[process];
    }

    return true;
}

bool createInitialSolutionG(int numberOfTasks,
                            int numberOfMachines,
                            int deadline,
                            int start[],
                            int duration[],
                            int machine[]) {
    int i, j;

    #pragma omp parallel for num_threads(4)
    for(j = 0; j < numberOfMachines; j++)
        freeTime[j] = deadline;

    int order[numberOfTasks];
    #pragma omp parallel for num_threads(4)
    for(i = 0; i < numberOfTasks; i++)
        order[i] = i;

    sorts(order, duration, numberOfTasks);

    int process, temp_machine = 0;
    for(i = 0; i < numberOfTasks; i++) {
        process = order[i];

        while(freeTime[temp_machine] < duration[process])
            temp_machine = (temp_machine + 1) % numberOfMachines;

        freeTime[temp_machine] = freeTime[temp_machine] - duration[process];
        start[process] = freeTime[temp_machine];
        machine[process] = temp_machine;

        temp_machine = (temp_machine + 1) % numberOfMachines;
    }

    int machineTime;
    for(j = 0; j < numberOfMachines; j++){
        machineTime = deadline;

        for(i = numberOfTasks - 1; i >= 0; i--) {
            process = order[i];

            if(machine[process] == j) {
                machineTime = machineTime - duration[process];
                start[process] = machineTime;
            }
        }
    }

    return true;
}

int valueOfSolution(int numberOfTasks,
                    int numberOfMachines,
                    int deadline,
                    int start[],
                    int duration[],
                    int machine[]) {
    int val = 0;

    for(int i = 0; i < numberOfTasks; i++)
        val = val + (deadline - duration[i] - start[i]);

    return val;
}

bool isInDeadlineInterval(int numberOfTasks, int deadline, int start[], int duration[]) {
    bool result = true;
    int i;

    #pragma omp parallel for private(i) num_threads(8)
    for(i = 0; i < numberOfTasks; i++) {
        if (start[i] < 0 || start[i] + duration[i] > deadline) {
            result = false;
            #pragma omp cancel for
        }

        #pragma omp cancellation point for
    }

    return result;
}

bool areThereConflicts(int numberOfTasks, int start[], int duration[], int machine[]) {
    bool result = true;
    int x, y;

    #pragma omp parallel for private(x) num_threads(8)
    for(x = 0; x < numberOfTasks; x++) {
        #pragma omp cancellation point for
        printf("Executando thread %d com x = %d\n", omp_get_thread_num(), x);

        #pragma omp parallel for private(y) num_threads(4)
        for(y = x + 1; y < numberOfTasks; y++) {
            printf("Executando thread %d com x = %d e y = %d\n", omp_get_thread_num(), x, y);

            if(machine[x] == machine[y]) {
                if (!(start[x] + duration[x] <= start[y] || start[y] + duration[y] <= start[x])) {
                    result = false;
                    #pragma omp cancel for
                }
            }

            #pragma omp cancellation point for
        }

        if(!result) {
            #pragma omp cancel for
        }
    }

    return result;
}

bool isValid(int numberOfTasks,
             int numberOfMachines,
             int deadline,
             int start[],
             int duration[],
             int machine[]) {

    bool result1, result2;
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            result1 = isInDeadlineInterval(numberOfTasks, deadline, start, duration);
        }

        #pragma omp section
        {
            result2 = areThereConflicts(numberOfTasks, start, duration, machine);
        }
    }

    return result1 && result2;
}

void printSolution(int numberOfTasks,
                   int numberOfMachines,
                   int deadline,
                   int start[],
                   int duration[],
                   int machine[]) {

    for(int i = 0; i < numberOfTasks; i++)
        printf("S[%d]: %d\n", i, start[i]);

    for(int j = 0; j < numberOfMachines; j++){
        printf("M%d: { ", j);

        for(int i = 0; i < numberOfTasks; i++)
            if(machine[i] == j)
                printf("%d ", i);

        printf("}\n");
    }
    printf("Value: %d\n", valueOfSolution(numberOfTasks, numberOfMachines, deadline, start, duration, machine));
}

void changeOrderRandomly(int n, int *random) {
    int aux, p1, p2;

    for(int i = 0; i < n * 30; i++) {
        p1 = rand() % n;
        p2 = rand() % n;

        aux = random[p1];
        random[p1] = random[p2];
        random[p2] = aux;
    }
}

bool generateRandomNeighbor(int numberOfTasks,
                            int numberOfMachines,
                            int deadline,
                            int start[],
                            int duration[],
                            int machine[]) {

    vector<int> sortedTasks;
    int process, machine_temp;
    int p1, p2;

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            changeOrderRandomly(numberOfMachines, randomMachines);
        }

        #pragma omp section
        {
            changeOrderRandomly(numberOfTasks, randomTasks);
        }
    }

    for(int i = 0; i < numberOfTasks; i++) {
        process = randomTasks[i];

        for(int j = 0; j < numberOfMachines; j++) {
            machine_temp = randomMachines[j];

            if(freeTime[machine_temp] >= duration[process] && machine_temp != machine[process]) {

                start[process] = freeTime[machine_temp] - duration[process];
                machine[process] = machine_temp;
                freeTime[machine_temp] = start[process];

                // Sort all jobs on the removed job machine
                sortedTasks.clear();
                for(int k = 0; k < numberOfTasks; k++) {

                    if(k != process && machine[k] == machine[process]) {
                        sortedTasks.push_back(k);
                        p1 = sortedTasks.size() - 1;

                        while(p1 > 0 && duration[sortedTasks[p1]] < duration[sortedTasks[p1-1]]) {
                            p2 = sortedTasks[p1];
                            sortedTasks[p1] = sortedTasks[p1-1];
                            sortedTasks[p1-1] = p2;
                            p1--;
                        }
                    }
                }

                freeTime[machine[process]] = deadline;

                for(int k = 0; k < sortedTasks.size(); k++){
                    start[sortedTasks[k]] = freeTime[machine[process]] - duration[sortedTasks[k]];
                    freeTime[machine[process]] = start[sortedTasks[k]];
                }

                return true;
            }
        }
    }

    return false;
}

bool shouldAcceptNeighbor(double delta, double temperature) {
    double roll = (double) (rand() % 10000) / 10000.0;
    return roll < pow(EULER, -(delta / temperature));
}

void simulatedAnnealing(int numberOfTasks,
                        int numberOfMachines,
                        int deadline,
                        int start[],
                        int duration[],
                        int machine[],
                        double temperature,
                        double decay,
                        int numberOfNeighbors,
                        char filename[]) {

    if(decay >= 1.0){
        printf("\nErro! Tentou rodar SA com esfriamento >= 1.0!\n");
        return;
    }

    int candidateStart[numberOfTasks];
    int candidateMachine[numberOfTasks];
    int candidateValue;

    int bestStart[numberOfTasks];
    int bestMachine[numberOfTasks];
    int bestValue;

    int i, j, x, y;

    #pragma omp parallel for num_threads(4)
    for(i = 0; i < numberOfTasks; i++){
        bestStart[i] = start[i];
        bestMachine[i] = machine[i];
    }

    const double limitTemperature = 0.01;
    int numberOfIterations = (int) (log2(limitTemperature / temperature) / log2(decay) + 1);

    int prevFreeTime[numberOfMachines];
    int currentValue;
    int count = 0;
    double delta;

    time_t t_start = time(0);
    struct tm *tinfo;
    time_t finish;

    printf("\nRodando Simulated Annealing com\n");
    printf("Temperatura inicial: %.2f\n", temperature);
    printf("Esfriamento: %.2f\n", decay);
    printf("Vizinhos a cada temperatura: %d\n\n", numberOfNeighbors);

    FILE *fp = fopen(filename, "w");

    while(temperature > limitTemperature) {

        currentValue = valueOfSolution(numberOfTasks, numberOfMachines, deadline, start, duration, machine);
        count++;

        printf("Iteracao: %d/%d\n", count, numberOfIterations);
        printf("Temperature: %.2f\n", temperature);
        printf("Solution Value: %d\n", currentValue);

        fprintf(fp, "%d, %d\n", count, currentValue);

        if(count > 1) {
            finish = ((time(0) - t_start) * numberOfIterations) / (count-1) + t_start;
            tinfo = localtime(&finish);

            printf("EFT: %02d/%02d/%02d - %02d:%02d:%02d\n",
                    tinfo->tm_mday,
                    tinfo->tm_mon + 1,
                    tinfo->tm_year + 1900,
                    tinfo->tm_hour,
                    tinfo->tm_min,
                    tinfo->tm_sec);
        }

        printf("\n");

        for(x = 0; x < numberOfNeighbors; x++) {

            #pragma omp parallel sections
            {
                #pragma omp section
                {
                    for(i = 0; i < numberOfTasks; i++) {
                        candidateStart[i] = start[i];
                        candidateMachine[i] = machine[i];
                    }
                }

                #pragma omp section
                {
                    for(j = 0; j < numberOfMachines; j++)
                        prevFreeTime[j] = freeTime[j];
                }
            }

            if(!generateRandomNeighbor(numberOfTasks, numberOfMachines, deadline, candidateStart, duration, candidateMachine))
                printf("Failed to generate Neighboor...\n");

            candidateValue = valueOfSolution(numberOfTasks, numberOfMachines, deadline, candidateStart, duration, candidateMachine);

            if(candidateValue < currentValue) {

                #pragma omp parallel for num_threads(4)
                for(i = 0; i < numberOfTasks; i++){
                    start[i] = candidateStart[i];
                    machine[i] = candidateMachine[i];
                }

                if(candidateValue < bestValue) {

                    #pragma omp parallel for num_threads(4)
                    for(i = 0; i < numberOfTasks; i++) {
                        bestStart[i] = start[i];
                        bestMachine[i] = machine[i];
                    }

                    bestValue = candidateValue;
                    printf("New Best! %d\n", bestValue);
                }

            } else {
                delta = candidateValue - currentValue;
                if(shouldAcceptNeighbor(delta, temperature)) {

                    #pragma omp parallel for num_threads(4)
                    for(i = 0; i < numberOfTasks; i++){
                        start[i] = candidateStart[i];
                        machine[i] = candidateMachine[i];
                    }

                } else {

                    #pragma omp parallel for num_threads(4)
                    for(j = 0; j < numberOfMachines; j++)
                        freeTime[j] = prevFreeTime[j];
                }
            }
        }

        temperature = temperature * decay;
    }

    fclose(fp);

    #pragma omp parallel for num_threads(4)
    for(int i = 0; i < numberOfTasks; i++){
        start[i] = bestStart[i];
        machine[i] = bestMachine[i];
    }
}


void saveSolution(int numberOfTasks,
                  int numberOfMachines,
                  int deadline,
                  int start[],
                  int duration[],
                  int machine[],
                  char filename[]) {

    FILE *fp = fopen(filename, "w");

    for(int i = 0; i < numberOfTasks; i++)
        fprintf(fp, "S[%d]: %d\n", i, start[i]);

    for(int j = 0; j < numberOfMachines; j++){
        fprintf(fp, "M%d: { ", j);

        for(int i = 0; i < numberOfTasks; i++)
            if(machine[i] == j)
                fprintf(fp, "%d ", i);

        fprintf(fp, "}\n");
    }

    fprintf(fp, "Value: %d\n", valueOfSolution(numberOfTasks, numberOfMachines, deadline, start, duration, machine));
    fclose(fp);
}
