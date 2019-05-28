#include <cstdio>
#include <vector>
#include <string.h>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <errno.h>

#define EULER 2.7182819

using namespace std;

int *freeTime;
int *randomOrder;
int *randomOrder2;

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

bool loadFile(int *n, int *m, int *d, vector<int> &S, vector<int> &P, vector<int> &M,  char fileName[]){

  FILE *fp = fopen(fileName, "r");

  if(!fp) return false;

  //Le m, n e d
  *m = readIntLine(fp);
  *n = readIntLine(fp);
  *d = readIntLine(fp);

  S.clear();
  P.clear();
  M.clear();

  for(int i = 0; i < *n; i++){
    S.push_back(0);
    P.push_back(readIntLine(fp));
    M.push_back(0);
  }

  fclose(fp);
  return true;
}

//Funcao auxiliar pra decidir a ordem que as tarefas sao colocadas nas maquinas
void sorts(vector<int> &order, vector<int> P){
  int j;
  int aux;
  for(int i = 1; i < order.size(); i++){
    j = i;
    while(j > 0 && P[j] > P[j-1]){
      aux = P[j];
      P[j] = P[j-1];
      P[j-1] = aux;

      aux = order[j];
      order[j] = order[j-1];
      order[j-1] = aux;

      j--;
    }
  }
}

void createInitialSolution(int n, int m, int d, vector<int> &S, vector<int> &P, vector<int> &M){
  int nextFree[m]; //Next Free time
  int choosenMachine;

  for(int j = 0; j < m; j++)
    nextFree[j] = d;

  vector<int> order;
  for(int i = 0; i < n; i++)
    order.push_back(i);

  sorts(order, P);

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

}

//Retorna o valor da solucao (Executa muito rapido)
int valueOfSolution(int n, int m, int d, vector<int> &S, vector<int> &P, vector<int> &M){
  int val = 0;

  for(int i = 0; i < n; i++)
    val = val + (d - P[i] - S[i]);

  return val;
}

//Verifica se uma solucao eh valida
bool isValid(int n, int m, int d, vector<int> &S, vector<int> &P, vector<int> &M){
  //Nenhuma tarefa termina depois do DeadLine ou comeca antes do tempo = 0
  for(int i = 0; i < n; i++){
    if(S[i] < 0 || S[i] + P[i] > d)
      return false;
  }

  //Nao ha conflitos (Demora pra avaliar isso aqui...)
  for(int x = 0; x < n; x++){
    for(int y = x+1; y < n; y++){
      if(M[x] == M[y]){
        if(!(S[x] + P[x] <= S[y] || S[y] + P[y] <= S[x])){
          return false;
        }
      }
    }
  }

  return true;
}

void printSolution(int n, int m, int d, vector<int> &S,  vector<int> &P,  vector<int> &M){
  for(int i = 0; i < n; i++)
    printf("S[%d]: %d\n", i, S[i]);

  for(int j = 0; j < m; j++){
    printf("M%d: { ", j);

    for(int i = 0; i < n; i++)
      if(M[i] == j)
        printf("%d ", i);

    printf("}\n");
  }
  printf("Value: %d\n", valueOfSolution(n, m, d, S, P, M));
}

//Tenta gerar uma solucao vizinha valida, retorna "true" se conseguir ou "false" se nao conseguir (Se funcionar, ela nunca retorna false...)
//Pode demorar um tempo consideravel, pois nem todo vizinho eh valido...
bool generateRamdomNeighboor(int n, int m, int d, vector<int> &S, vector<int> &P, vector<int> &M, double* delta){
  bool cantPlace;
  int p1, p2, aux;

  int cT, cM;

  vector<int> sortedMachine;

  for(int i = 0; i < m*30; i++){
    p1 = rand()%m;
    p2 = rand()%m;

    aux = randomOrder[p1];
    randomOrder[p1] = randomOrder[p2];
    randomOrder[p2] = aux;
  }

  for(int i = 0; i < n*30; i++){
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
        sortedMachine.clear();
        for(int k = 0; k < n; k++){
          if(k != cT && M[k] == M[cT]){
            sortedMachine.push_back(k);
            p1 = sortedMachine.size() - 1;
            while(p1 > 0 && P[sortedMachine[p1]] < P[sortedMachine[p1-1]]){
              p2 = sortedMachine[p1];
              sortedMachine[p1] = sortedMachine[p1-1];
              sortedMachine[p1-1] = p2;
              p1--;
            }
          }
        }

        freeTime[M[cT]] = d;
        for(int k = 0; k < sortedMachine.size(); k++){
          *delta = *delta + S[sortedMachine[k]];
          S[sortedMachine[k]] = freeTime[M[cT]] - P[sortedMachine[k]];
          *delta = *delta - S[sortedMachine[k]];
          freeTime[M[cT]] = S[sortedMachine[k]]; //freeTime[M[cT]] - P[sortedMachine[k]];
        }

        M[cT] = cM;

        return true;
      }
    }
  }
  return false;
}

//Aceita um viznho pior aleatoriamente
bool shouldAccept(double delta, double temperature){
  double roll = (double) (rand()%10000)/10000.0;

  if(roll < pow(EULER, -(delta/temperature)))
    return true;
  else
    return false;
}

//Simulated Annealing
void simulatedAnnealing(int n, int m, int d, vector<int> &S,  vector<int> &P,  vector<int> &M, double temperature, double decay, int nVizinhos, char fileName[]){
  vector<int> Sl;
  vector<int> Ml;
  int prevFreeTime[m];
  const double limitTemperature = 0.01;

  double delta;

  if(decay >= 1.0){
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
    printf("Temperature: %.2f\nSolution Value: %d\n", temperature, valueOfSol);
    printf("Iteracao: %d/%d\n", count,nItr);
    fprintf(fp, "%d, %d\n", count, valueOfSol);

    if(count > 1){
      finish = ((time(0) - start) * nItr) / (count-1) + start;
      tinfo = localtime(&finish);

      printf("EFT: %02d/%02d/%02d - %02d:%02d:%02d\n", tinfo->tm_mday, tinfo->tm_mon + 1, tinfo->tm_year + 1900, tinfo->tm_hour, tinfo->tm_min, tinfo->tm_sec);
    }
    printf("\n");

    for(int i = 0; i < nVizinhos; i++){
      Sl = S;
      Ml = M;

      for(int j = 0; j < m; j++)
        prevFreeTime[j] = freeTime[j];

      if(!generateRamdomNeighboor(n, m, d, Sl, P, Ml, &delta))
        printf("Failed to generate Neighboor...\n");

      if(delta <= 0){
        S = Sl;
        M = Ml;
      }
      else{
        if(shouldAccept(delta, temperature)){
          S = Sl;
          M = Ml;
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
}

//Salva a solucao resultado em um arquivo
void saveSolution(int n, int m, int d, vector<int> &S,  vector<int> &P,  vector<int> &M, char fileName[]){
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
  fprintf(fp, "Value: %d\n", valueOfSolution(n, m, d, S, P, M));

  fclose(fp);
}

int vmax(int a, int b){
  return (a > b) ? a : b;
}

int main(int argc, char* argv[]){
  //Entradas
  int n, m, d;
  vector<int> P;

  //Saida
  vector<int> S;
  vector<int> M;

  srand(time(0));

  if(argc != 2){
    printf("Informe o arquivo de entrada\n");
    return 0;
  }

  //Le arquivo de entrada
  if(!loadFile(&n, &m, &d, S, P, M, argv[1])){
    printf("Nao pode abrir %s\n", argv[1]);
    return 0;
  }

  printf("Lido: \nN: %d\nM: %d\nD: %d\n", n, m, d);

  freeTime = new int[m];
  randomOrder = new int[m];
  randomOrder2 = new int[n];

  for(int i = 0; i < m; i++)
    randomOrder[i] = i;
  for(int i = 0; i < n; i++)
    randomOrder2[i] = i;

  //Cria a solucao inicial
  createInitialSolution(n, m, d, S, P, M);

  printf("Criada solucao inicial com valor: %d\n", valueOfSolution(n, m, d, S, P, M));
  if(isValid(n, m, d, S, P, M))
    printf("A solucao inicial eh valida\n");
  else{
    printf("ERRO: Solucao inicial invalida!\n");
    return 0;
  }

  //Arquivo CSV
  char nomeArq[200];
  strcpy(nomeArq, argv[1]);
  strcpy(strstr(nomeArq, "."), "Solution.csv");

  //Roda o simulated annealing
  simulatedAnnealing(n, m, d, S, P, M, valueOfSolution(n, m, d, S, P, M)/20, 0.9, vmax(n/4, 50), nomeArq);

  //Informa o valor da nova solucao
  printf("Novo Valor: %d\n", valueOfSolution(n, m, d, S, P, M));
  if(isValid(n, m, d, S, P, M))
    printf("A solucao final eh valida\n");
  else{
    printf("ERRO: Solucao final invalida!\n");
    return 0;
  }

  //Salva a solucao em um arquivo
  strcpy(nomeArq, argv[1]);
  strcpy(strstr(nomeArq, "."), "Solution.txt");
  saveSolution(n, m, d, S, P, M, nomeArq);

  printf("\nResultado salvo em: %s\n", nomeArq);

  delete[] freeTime;
  delete[] randomOrder;
  delete[] randomOrder2;

  return 0;
}
