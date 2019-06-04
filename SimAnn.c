#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define EULER 2.7182819

int *freeTime;
int *randomOrder;
int *randomOrder2;

int *sortedMachine; //Used by neighboor generator

int intFromString(char str[]){
  int resultado = 0, i;
  for(i = 0; i < strlen(str); i++)
    resultado = resultado * 10 + (str[i] - '0');

  return resultado;
}

int readIntLine(FILE *fp){
  char numLido[20];
  char c;
  int i = 0;

  c = fgetc(fp);
  while(c != '\n'){
    numLido[i] = c;
    i++;
    c = fgetc(fp);
  }
  numLido[i] = '\0';

  return intFromString(numLido);
}

int loadFile(int *n, int *m, int *d, int **S, int **P, int **M){

  //Le m, n e d
  *m = readIntLine(stdin);
  *n = readIntLine(stdin);
  *d = readIntLine(stdin);

  (*S) = (int*) malloc((*n) * sizeof(int));
  (*P) = (int*) malloc((*n) * sizeof(int));
  (*M) = (int*) malloc((*n) * sizeof(int));

  for(int i = 0; i < *n; i++){
    (*S)[i] = 0;
    (*M)[i] = 0;
    (*P)[i] = readIntLine(stdin);
  }

  return 1;
}

//Funcao auxiliar pra decidir a ordem que as tarefas sao colocadas nas maquinas
void sorts(int order[], int n, int P[]){
  int j;
  int aux;

  int cpyP[n];

  for(int i = 0; i < n; i++)
    cpyP[i] = P[i];

  for(int i = 1; i < n; i++){
    j = i;
    while(j > 0 && cpyP[j] > cpyP[j-1]){
      aux = cpyP[j];
      cpyP[j] = cpyP[j-1];
      cpyP[j-1] = aux;

      aux = order[j];
      order[j] = order[j-1];
      order[j-1] = aux;

      j--;
    }
  }
}

//Cria uma solucao inicial (Ruim)
int createInitialSolution(int n, int m, int d, int S[], int P[], int M[]){
  int nextFree[m]; //Next Free time
  int choosenMachine;

  for(int j = 0; j < m; j++)
    nextFree[j] = d;

  int order[n];

  for(int i = 0; i < n; i++)
    order[i] = i;

  sorts(order, n, P);

  for(int i = 0; i < n; i++){
    choosenMachine = 0;

    while(nextFree[choosenMachine] - P[order[i]] < 0 && choosenMachine < m)
      choosenMachine++;

    if(choosenMachine == m){
      choosenMachine = 0;
    }

    S[order[i]] = nextFree[choosenMachine] - P[order[i]];
    M[order[i]] = choosenMachine;
    nextFree[choosenMachine] = nextFree[choosenMachine] - P[order[i]];
  }

  for(int j = 0; j < m; j++)
    freeTime[j] = nextFree[j];

  return 1;
}

//Cria uma solucao inicial (Boa)
int createInitialSolutionG(int n, int m, int d, int S[], int P[], int M[]){
  for(int j = 0; j < m; j++)
    freeTime[j] = d;

  int order[n];

  for(int i = 0; i < n; i++)
    order[i] = i;

  sorts(order, n, P);

  int k = 0;
  for(int i = 0; i < n; i++){
      while(freeTime[k] < P[order[i]]){
        k++;
        if(k >= m) k = 0;
      }
      S[order[i]] = freeTime[k] - P[order[i]];
      freeTime[k] = freeTime[k] - P[order[i]];
      M[order[i]] = k;
      k++;
      if(k >= m) k = 0;
  }

  int tim;
  for(int j = 0; j < m; j++){
    tim = d;
    for(int i = n-1; i >= 0; i--){
      if(M[order[i]] == j){
        S[order[i]] = tim - P[order[i]];
        tim = tim - P[order[i]];
      }
    }
  }

  return 1;
}

//Retorna o valor da solucao
int valueOfSolution(int n, int m, int d, int S[], int P[], int M[]){
  int val = 0;

  for(int i = 0; i < n; i++)
    val = val + (d - P[i] - S[i]);

  return val;
}

//Verifica se uma solucao eh valida
int isValid(int n, int m, int d, int S[], int P[], int M[]){
  //Nenhuma tarefa termina depois do DeadLine ou comeca antes do tempo = 0
  for(int i = 0; i < n; i++){
    if(S[i] < 0 || S[i] + P[i] > d)
      return 0;
  }

  //Nao ha conflitos
  for(int x = 0; x < n; x++){
    for(int y = x+1; y < n; y++){
      if(M[x] == M[y]){
        if(!(S[x] + P[x] <= S[y] || S[y] + P[y] <= S[x])){
          return 0;
        }
      }
    }
  }

  return 1;
}

void printSolution(int n, int m, int d, int S[], int P[], int M[]){
  for(int i = 0; i < n; i++)
    printf("S[%d]: %d\n", i, S[i]);

  for(int j = 0; j < m; j++){
    printf("M%d: { ", j);

    for(int i = 0; i < n; i++)
      if(M[i] == j)
        printf("%d ", i);

    printf("}\n");
  }
  printf("Valor: %d\n", valueOfSolution(n, m, d, S, P, M));
}

//Tenta gerar uma solucao vizinha valida, retorna "true" se conseguir ou "false" se nao conseguir (Se funcionar, ela nunca retorna false...)
//Pode demorar um tempo consideravel, pois nem todo vizinho eh valido...
int generateRamdomNeighboor(int n, int m, int d, int S[], int P[], int M[], double* delta){
  int cantPlace;
  int p1, p2, aux;

  int cT, cM;

  int smSize;

  for(int i = 0; i < m*20; i++){
    p1 = rand()%m;
    p2 = rand()%m;

    aux = randomOrder[p1];
    randomOrder[p1] = randomOrder[p2];
    randomOrder[p2] = aux;
  }

  for(int i = 0; i < n*20; i++){
    p1 = rand()%n;
    p2 = rand()%n;

    aux = randomOrder2[p1];
    randomOrder2[p1] = randomOrder2[p2];
    randomOrder2[p2] = aux;
  }

  for(int i = 0; i < n; i++){
    cT = randomOrder2[i];
    for(int j = 0; j < m; j++){
      cM = randomOrder[j];
      if(freeTime[cM] >= P[cT] && cM != M[cT]){

        *delta = S[cT];
        S[cT] = freeTime[cM] - P[cT];
        *delta = *delta - S[cT];
        freeTime[cM] = S[cT]; //freeTime[cM] - P[cT]

        //Sorts all Jobs on the removed job machine
        smSize = 0;
        for(int k = 0; k < n; k++){
          if(M[k] == M[cT] && k != cT){
            sortedMachine[smSize] = k;
            p1 = smSize;
            smSize++;
            while(p1 > 0 && P[sortedMachine[p1]] < P[sortedMachine[p1-1]]){
              p2 = sortedMachine[p1];
              sortedMachine[p1] = sortedMachine[p1-1];
              sortedMachine[p1-1] = p2;
              p1--;
            }
          }
        }

        freeTime[M[cT]] = d;
        for(int k = 0; k < smSize; k++){
          *delta = *delta + S[sortedMachine[k]];
          S[sortedMachine[k]] = freeTime[M[cT]] - P[sortedMachine[k]];
          *delta = *delta - S[sortedMachine[k]];
          freeTime[M[cT]] = S[sortedMachine[k]]; //freeTime[M[cT]] - P[sortedMachine[k]];
        }

        M[cT] = cM;

        smSize = 0;
        for(int k = 0; k < n; k++){
          if(M[k] == cM){
            sortedMachine[smSize] = k;
            p1 = smSize;
            smSize++;
            while(p1 > 0 && P[sortedMachine[p1]] < P[sortedMachine[p1-1]]){
              p2 = sortedMachine[p1];
              sortedMachine[p1] = sortedMachine[p1-1];
              sortedMachine[p1-1] = p2;
              p1--;
            }
          }
        }

        freeTime[cM] = d;
        for(int k = 0; k < smSize; k++){
          *delta = *delta + S[sortedMachine[k]];
          S[sortedMachine[k]] = freeTime[cM] - P[sortedMachine[k]];
          *delta = *delta - S[sortedMachine[k]];
          freeTime[cM] = S[sortedMachine[k]];
        }

        return 1;
      }
    }
  }
  return 0;
}

//Aceita um viznho pior aleatoriamente
int shouldAccept(double delta, double temperature){
  double roll = (double) (rand()%100000)/100000.0;

  if(roll < pow(EULER, -(delta/temperature)))
    return 1;
  else
    return 0;
}

//Simulated Annealing
void simulatedAnnealing(int n, int m, int d, int S[], int P[], int M[], double temperature, double decay, int nVizinhos, char fileName[]){
  int Sl[n];
  int Ml[n];
  int Sb[n];
  int Mb[n];

  for(int i = 0; i < n; i++){
    Sb[i] = S[i];
    Mb[i] = M[i];
  }

  int valB = valueOfSolution(n, m, d, S, P, M);
  int valA = valB;

  int prevFreeTime[m];
  const double limitTemperature = 0.01;

  double delta;

  if(decay >= 0.99999){
    printf("\nErro! Tentou rodar SA com esfriamento >= 1.0!\n");
    return;
  }

  int nItr = (int) (log2(limitTemperature/temperature) / log2(decay) + 1); //Numero de vezes que a temperatura vai cair
  int count = 0;
  int valueOfSol;

  time_t start = time(0);
  time_t finish;
  struct tm *tinfo;

  FILE *fp = fopen(fileName, "w");

  printf("\nRodando Simulated Annealing com:\nTemperatura inicial: %.2f\nEsfriamento: %.2f\nVizinhos a cada Temperatura: %d\n\n", temperature, decay, nVizinhos);
  while(temperature > limitTemperature){

    count++;
    valueOfSol = valueOfSolution(n, m, d, S, P, M);
    printf("Temperatura: %.2f\nValor da Solucao: %d\n", temperature, valueOfSol);
    printf("Iteracao: %d/%d\n", count,nItr);
    fprintf(fp, "%d, %d\n", count, valueOfSol);

    if(count > 1){
      finish = ((time(0) - start) * nItr) / (count-1) + start;
      tinfo = localtime(&finish);

      printf("EFT: %02d/%02d/%02d - %02d:%02d:%02d\n", tinfo->tm_mday, tinfo->tm_mon + 1, tinfo->tm_year + 1900, tinfo->tm_hour, tinfo->tm_min, tinfo->tm_sec);
    }
    printf("\n");

    for(int x = 0; x < nVizinhos; x++){
      for(int i = 0; i < n; i++){
        Sl[i] = S[i];
        Ml[i] = M[i];
      }

      for(int j = 0; j < m; j++)
        prevFreeTime[j] = freeTime[j];

      if(!generateRamdomNeighboor(n, m, d, Sl, P, Ml, &delta))
        printf("Failed to generate Neighboor...\n");

      if(delta <= 0){
        valA = valA + (int) delta;

        for(int i = 0; i < n; i++){
          S[i] = Sl[i];
          M[i] = Ml[i];
        }

        if(valA < valB){
            for(int i = 0; i < n; i++){
              Sb[i] = S[i];
              Mb[i] = M[i];
            }
            valB = valA;
            printf("Novo Melhor! Valor: %d\n", valA);
        }
      }
      else{
        if(shouldAccept(delta, temperature)){
          for(int i = 0; i < n; i++){
            S[i] = Sl[i];
            M[i] = Ml[i];
          }
          valA = valA + (int) delta;
        }
        else{
          for(int j = 0; j < m; j++)
           freeTime[j] = prevFreeTime[j];
        }
      }
    }
    temperature = temperature * decay;
  }
  fclose(fp);

  for(int i = 0; i < n; i++){
    S[i] = Sb[i];
    M[i] = Mb[i];
  }
}

//Salva a solucao resultado em um arquivo
void saveSolution(int n, int m, int d, int S[], int P[], int M[], char fileName[]){
  FILE *fp = fopen(fileName, "w");

  for(int i = 0; i < n; i++)
    fprintf(fp, "S[%d]: %d\n", i, S[i]);

  for(int j = 0; j < m; j++){
    fprintf(fp, "M%d: { ", j);

    for(int i = 0; i < n; i++)
      if(M[i] == j)
        fprintf(fp, "%d ", i);

    fprintf(fp, "}\n");
  }
  fprintf(fp, "Valor: %d\n", valueOfSolution(n, m, d, S, P, M));

  fclose(fp);
}

int vmax(int a, int b){
  return (a > b) ? a : b;
}

int main(int argc, char* argv[]){
  //Entradas
  int n, m, d, initialValue;
  int* P;

  //Saida
  int* S;
  int* M;

  if(argc != 3){
    printf("Parametros da linha de comando: ARQUIVO_DE_SAIDA SEED\n");
    return 0;
  }

  int seed = intFromString(argv[2]);
  if(seed == 0){
    seed = time(0);
  }

  srand(seed);

  //Le arquivo de entrada
  loadFile(&n, &m, &d, &S, &P, &M);

  freeTime = (int*) malloc(m * sizeof(int));
  randomOrder = (int*) malloc(m * sizeof(int));
  randomOrder2 = (int*) malloc(n * sizeof(int));
  sortedMachine = (int*) malloc(n * sizeof(int));

  for(int i = 0; i < m; i++)
    randomOrder[i] = i;
  for(int i = 0; i < n; i++)
    randomOrder2[i] = i;

  //Cria a solucao inicial
  createInitialSolution(n, m, d, S, P, M);
  initialValue = valueOfSolution(n, m, d, S, P, M);

  printf("Seed utilizada: %d\n", seed);
  printf("Criada solucao inicial com valor: %d\n", initialValue);
  if(!isValid(n, m, d, S, P, M)){
    printf("ERRO: Solucao inicial invalida!\n");
    return 0;
  }

  printf("\nSolucao inicial:\n");
  printSolution(n, m, d, S, P, M);

  //Arquivo CSV
  char nomeArq[200];
  strcpy(nomeArq, argv[1]);
  if(strstr(nomeArq, "."))
    strcpy(strstr(nomeArq, "."), ".csv");
  else
    strcat(nomeArq, ".csv");

  //Roda o simulated annealing
  simulatedAnnealing(n, m, d, S, P, M, vmax(10, valueOfSolution(n, m, d, S, P, M)/2800), 0.9, vmax(n*25, 50), nomeArq);

  //Informa o valor da nova solucao
  if(!isValid(n, m, d, S, P, M)){
    printf("ERRO: Solucao final invalida!\n");
    return 0;
  }

  printf("Solucao Final:\n");
  printSolution(n, m, d, S, P, M);

  //Salva a solucao em um arquivo
  saveSolution(n, m, d, S, P, M, argv[1]);

  printf("\nResultado salvo em: %s\n", argv[1]);

  free(freeTime);
  free(randomOrder);
  free(randomOrder2);
  free(sortedMachine);
  free(S);
  free(P);
  free(M);

  return 0;
}
