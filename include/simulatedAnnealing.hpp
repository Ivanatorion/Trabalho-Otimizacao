#ifndef TRABALHO_OTIMIZACAO_SIMULATED_ANNEALING_HPP
#define TRABALHO_OTIMIZACAO_SIMULATED_ANNEALING_HPP

void sorts(int index[], int duration[], int numberOfTasks);

bool createInitialSolution(int numberOfTasks,
                           int numberOfMachines,
                           int deadline,
                           int start[],
                           int duration[],
                           int machine[]);

bool createInitialSolutionG(int numberOfTasks,
                            int numberOfMachines,
                            int deadline,
                            int start[],
                            int duration[],
                            int machine[]);

int valueOfSolution(int numberOfTasks,
                    int numberOfMachines,
                    int deadline,
                    int start[],
                    int duration[],
                    int machine[]);

bool isInDeadlineInterval(int numberOfTasks, int deadline, int start[], int duration[]);

bool areThereConflicts(int numberOfTasks, int start[], int duration[], int machine[]);

bool isValid(int numberOfTasks,
             int numberOfMachines,
             int deadline,
             int start[],
             int duration[],
             int machines[]);

void printSolution(int numberOfTasks,
                   int numberOfMachines,
                   int deadline,
                   int start[],
                   int duration[],
                   int machine[]);

void changeOrderRandomly(int n, int *random);

bool generateRandomNeighbor(int numberOfTasks,
                            int numberOfMachines,
                            int deadline,
                            int start[],
                            int duration[],
                            int machine[]);

bool shouldAcceptNeighbor(double delta, double temperature);

void simulatedAnnealing(int numberOfTasks,
                        int numberOfMachines,
                        int deadline,
                        int start[],
                        int duration[],
                        int machine[],
                        double temperature,
                        double decay,
                        int numberOfNeighbors,
                        char filename[]);

void saveSolution(int numberOfTasks,
                  int numberOfMachines,
                  int deadline,
                  int start[],
                  int duration[],
                  int machine[],
                  char filename[]);

#endif //TRABALHO_OTIMIZACAO_SIMULATED_ANNEALING_HPP
