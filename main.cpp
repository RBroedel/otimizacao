#include <iostream>
#include <string>
#include <dirent.h>
#include <fstream>
#include <list>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <cstring>

using namespace std;

#define NODE 101
#define ROTAS 14
#define INF 99999

class Instance
{
public:
  string name;
  int dimension;
  int capacity;
  int trucks;
  int x[NODE];
  int y[NODE];
  double distancia[(NODE * (NODE - 1)) / 2];
  int demand[NODE];
};

class Solucao
{
public:
  int cost;
  int trucks;
  int rotas[ROTAS][NODE];
  int ocupacaoRota[ROTAS];
  double tempo_melhor;
  double tempo_medio;
  double tempo_total;
};

double matrizDistancia[NODE][NODE];
int custoOrdenado[NODE];
Instance instance;

void getIndexesNodePositions(int positions[], string line);
int getNodeDemand(string line);
void printInstace(Instance instance);
void printSolution(Solucao solution);
void getDimensionAndTrucksFromName(string name, int *dimension, int *instanceTrucks);
string getFileToRead();
string current_working_directory();
void readInstance(string directory, Instance *instance);
void heuConGul(Solucao &s, Instance inst);
double calcDistancia(int x1, int x2, int y1, int y2);
void ordenaCusto(int demand[], int dimensao);
void montarMatrizDistancia(Instance);
void montarMatrizDistanciaInstE134(Instance);
void calculoFO(Solucao &s);
void clonarsolucao(Solucao &sOri, Solucao &sClo);
void escreverArquivo(Solucao solucao, int iteracao);
string readSolutionFile(string nome);
void ils(const double tempo_max, Solucao &s, Instance inst, double &tempo_melhor, double &tempo_total);
void heuBLPM(Solucao &s);
void gerarVizinha(Solucao &s_vizinha);

int main(int argc, char *argv[])
{
  clock_t h = 0;
  Solucao solucao;
  string directoryFiles = current_working_directory() + "/";
  double tempo_melhor, tempo_total;

  instance.name = getFileToRead();
  directoryFiles += instance.name + "/";
  getDimensionAndTrucksFromName(instance.name, &instance.dimension, &instance.trucks);
  readInstance(directoryFiles, &instance);
  ordenaCusto(instance.demand, instance.dimension);
  if (instance.name == "E-n13-k4")
  {
    montarMatrizDistanciaInstE134(instance);
  }
  else
  {
    montarMatrizDistancia(instance);
  }

  //printInstace(instance);
  for (int i = 1; i < 4; i++)
  {
    heuConGul(solucao, instance);
    calculoFO(solucao);
    ils(120, solucao, instance, tempo_melhor, tempo_total);
    escreverArquivo(solucao, i);
  }

  //auto teste = readSolutionFile(instance.name);
  return 0;
}

void getIndexesNodePositions(int positions[], string line)
{
  int spaceOrder = 0;
  if (!(line[0] == ' '))
  {
    spaceOrder++;
  }

  for (int i = 0; i < line.length(); i++)
  {
    if (spaceOrder == 0 && line[i] == ' ')
    {
      spaceOrder++;
    }
    else if (spaceOrder > 0 && line[i] == ' ')
    {
      positions[spaceOrder - 1] = i + 1;
      spaceOrder++;
    }
    else if (spaceOrder > 2)
    {
      return;
    }
  }
}

int getNodeDemand(string line)
{
  for (int i = 0; i < line.length(); i++)
  {
    if (line[i] == ' ')
    {
      return stoi(line.substr((i + 1), line.length()));
    }
  }
  return 0;
}

void printInstace(Instance instance)
{
  cout << "name: " << instance.name << " dimension: " << instance.dimension << " trucks: " << instance.trucks << endl;
  for (int i = 0; i < instance.dimension; i++)
  {
    cout << "node: " << instance.x[i] << " " << instance.y[i] << " " << instance.demand[i] << endl;
  }
}

void printSolution(Solucao solution)
{
  for (int i = 0; i < solution.trucks; i++)
  {
    cout << "\nRoute #" << i + 1 << ": ";
    for (int j = 0; solution.rotas[i][j] != -1; j++)
    {
      cout << solution.rotas[i][j] << " ";
    }

    cout << "Ocupacao: " << solution.ocupacaoRota[i];
  }
  cout << "\nCusto: " << solution.cost << endl;
  cout << "Tempo melhor: " << solution.tempo_melhor << endl;
  cout << "Tempo medio: " << solution.tempo_medio << endl;
  cout << "Tempo total: " << solution.tempo_total << endl;
}

void getDimensionAndTrucksFromName(string name, int *dimension, int *instanceTrucks)
{
  int startDimension;
  for (int i = 0; i < name.size(); i++)
  {
    if (name[i] == 'n')
    {
      startDimension = i + 1;
    }
    if (name[i] == 'k')
    {
      *dimension = stoi(name.substr(startDimension, i));
      *instanceTrucks = stoi(name.substr(i + 1, name.size()));
      return;
    }
  }
}

string getFileToRead()
{
  int seletedOption = -1;
  string instances[8] = {"A-n32-k5", "A-n80-k10", "B-n44-k7", "B-n64-k9", "E-n101-k14", "E-n13-k4", "P-n101-k4", "P-n20-k2"};
  while (seletedOption < 1 || seletedOption > 8)
  {
    cout << "Input number of instance to be considered:\n";
    for (int i = 0; i < 8; i++)
    {
      cout << i + 1 << " - " << instances[i] << "\n";
    }
    cin >> seletedOption;
    if (seletedOption < 1 || seletedOption > 8)
    {
      cout << "Invalid option!\n";
    }
  };
  return instances[seletedOption - 1];
}

string current_working_directory()
{
  char *cwd = getcwd(0, 0);
  string working_directory(cwd);
  free(cwd);
  return working_directory;
}

void readInstance(string directory, Instance *instance)
{
  int isLendoInstanciaE13 = instance->name == "E-n13-k4";
  int isReadingNodes = 0;
  int isReadingDemand = 0;
  int index = 0;
  int positions[2];
  ifstream file(directory + "instance.txt");
  if (file.is_open())
  {
    string line;
    while (getline(file, line))
    {
      if (line.find("CAPACITY : ") == 0)
      {
        instance->capacity = stoi(line.substr(10, line.length()));
        continue;
      }
      if (line.find("NODE_COORD_SECTION") == 0 || line.find("EDGE_WEIGHT_SECTION") == 0)
      {
        isReadingNodes = 1;
        continue;
      }
      if (isReadingNodes)
      {
        if (line.find("DEMAND_SECTION") == 0)
        {
          isReadingNodes = 0;
          isReadingDemand = 1;
          index = 0;
          continue;
        }
        else
        {
          if (isLendoInstanciaE13)
          {
            instance->distancia[index] = stoi(line);
          }
          else
          {
            getIndexesNodePositions(positions, line);
            instance->x[index] = stoi(line.substr(positions[0], positions[1] - 1));
            instance->y[index] = stoi(line.substr(positions[1], line.length()));
          }
          index++;
        }
      }
      if (isReadingDemand)
      {
        if (line.find("DEPOT_SECTION") == 0)
        {
          isReadingDemand = 0;
        }
        else
        {
          instance->demand[index] = getNodeDemand(line);
          custoOrdenado[index] = index;
          index++;
        }
      }
    }
    file.close();
  }
}

double calcDistancia(int x1, int x2, int y1, int y2)
{
  return sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));
}

void ordenaCusto(int demand[], int dimensao)
{
  int i, troca = 1, aux;
  while (troca == 1)
  {
    troca = 0;
    for (i = 0; i < dimensao - 1; i++)
    {
      if (demand[custoOrdenado[i]] < demand[custoOrdenado[i + 1]])
      {
        troca = 1;
        aux = custoOrdenado[i];
        custoOrdenado[i] = custoOrdenado[i + 1];
        custoOrdenado[i + 1] = aux;
      }
    }
  }
}

void heuConGul(Solucao &s, Instance inst)
{
  s.trucks = inst.trucks;
  memset(s.rotas, -1, sizeof(s.rotas));
  memset(s.ocupacaoRota, 0, sizeof(s.ocupacaoRota));

  int caminhaoDesignarPonto = 0;
  int round = 0;

  for (int i = 0; i < inst.dimension - 1; i++)
  {
    for (int j = caminhaoDesignarPonto; j < s.trucks; j++)
    {
      if (s.ocupacaoRota[j] + inst.demand[custoOrdenado[i]] <= inst.capacity)
      {
        s.rotas[j][round] = custoOrdenado[i];
        s.ocupacaoRota[j] += inst.demand[custoOrdenado[i]];
        caminhaoDesignarPonto = j + 1;
        if (j + 1 == s.trucks)
        {
          round++;
          caminhaoDesignarPonto = 0;
        }
        goto foiDesignado;
      }
    }
  foiDesignado:;
  }
}

void montarMatrizDistancia(Instance inst)
{
  int i, j;
  double distanciaAux;
  memset(matrizDistancia, -1, sizeof(matrizDistancia));
  for (i = 0; i < inst.dimension; i++)
  {
    matrizDistancia[i][i] = INF;
  }
  for (i = 0; i < inst.dimension - 1; i++)
  {
    for (j = i + 1; j < inst.dimension; j++)
    {
      distanciaAux = calcDistancia(inst.x[i], inst.x[j], inst.y[i], inst.y[j]);
      matrizDistancia[i][j] = matrizDistancia[j][i] = distanciaAux;
    }
  }

  //PRINTA MATRIZ DE CUSTO DAS INSTANCIAS EXCETO E-N13-44
  // for(i = 0; i < inst.dimension; i++) {
  //   for(j = 0; j < inst.dimension; j++) {
  //     cout << matrizDistancia[i][j] << " ";
  //   }
  //   cout << endl;
  // }
}

void montarMatrizDistanciaInstE134(Instance inst)
{
  int i, j, k = 0;
  double distanciaAux;
  memset(matrizDistancia, -1, sizeof(matrizDistancia));
  for (i = 0; i < inst.dimension; i++)
  {
    matrizDistancia[i][i] = INF;
  }
  for (i = 1; i < inst.dimension; i++)
  {
    for (j = 0; j < i; j++)
    {
      matrizDistancia[i][j] = matrizDistancia[j][i] = inst.distancia[k++];
    }
  }

  //PRINTA MATRIZ DE CUSTO DA INSTANCIA E-N13-44
  // for (i = 0; i < inst.dimension; i++)
  // {
  //   for (j = 0; j < inst.dimension; j++)
  //   {
  //     cout << matrizDistancia[i][j] << " ";
  //   }
  //   cout << endl;
  // }
}

void calculoFO(Solucao &s)
{
  int j;
  s.cost = 0;
  for (int i = 0; i < s.trucks; i++)
  {
    s.ocupacaoRota[i] = 0;
    s.cost += matrizDistancia[0][s.rotas[i][0]];
    for (j = 0; s.rotas[i][j + 1] != -1; j++)
    {
      s.ocupacaoRota[i] += instance.demand[s.rotas[i][j]];
      s.cost += matrizDistancia[s.rotas[i][j]][s.rotas[i][j + 1]];
    }
    s.ocupacaoRota[i] += instance.demand[s.rotas[i][j]];
    s.cost += matrizDistancia[s.rotas[i][j]][0];
    if (s.ocupacaoRota[i] > instance.capacity)
    {
      s.cost = s.cost * 10;
    }
  }
}

void clonarsolucao(Solucao &sOri, Solucao &sClo)
{
  memcpy(&sClo, &sOri, sizeof(sOri));
}

void escreverArquivo(Solucao solucao, int iteracao)
{
  string saida = instance.name + "-" + "saida" + to_string(iteracao) + ".txt";
  FILE *f = fopen(saida.c_str(), "w");

  if (f == NULL)
  {
    perror("Erro ao abrir o arquivo.\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < solucao.trucks; i++)
  {
    fprintf(f, "%s%d%s", "Route # ", i + 1, ": ");
    for (int j = 0; solucao.rotas[i][j] != -1; j++)
    {
      fprintf(f, "%d%s", solucao.rotas[i][j], " ");
    }
    fprintf(f, "%s%d\n", "Ocupacao: ", solucao.ocupacaoRota[i]);
  }
  fprintf(f, "%s%d\n", "Custo: ", solucao.cost);
  fprintf(f, "%s%f\n", "Tempo Melhor: ", solucao.tempo_melhor);
  fprintf(f, "%s%f\n", "Tempo Medio: ", solucao.tempo_medio);
  fprintf(f, "%s%f\n", "Tempo Total: ", solucao.tempo_total);

  fclose(f);
}

string readSolutionFile(string nome)
{
  string line;
  auto aux = "./" + nome + "/solution.txt";
  ifstream myfile(aux);
  cout << "\nPrintando Solução da Instância: " + nome + "\n\n";
  if (myfile.is_open())
  {
    while (getline(myfile, line))
    {
      cout << line << endl;
    }
    myfile.close();
    return "Solução da instância: " + nome + " lida com sucesso!\n";
  }
  else
  {
    cout << "Unable to open file";
    return 0;
  }
}

void ils(const double tempo_max, Solucao &s, Instance inst, double &tempo_melhor, double &tempo_total)
{
  clock_t hI, hF;
  Solucao s_vizinha;
  int i = 0;
  hI = clock();
  heuConGul(s, inst);
  calculoFO(s);
  heuBLPM(s);
  hF = clock();
  tempo_melhor = ((double)(hF - hI)) / CLOCKS_PER_SEC;
  tempo_total = tempo_melhor;
  i++;
  while (tempo_total < tempo_max)
  {
    memcpy(&s_vizinha, &s, sizeof(s));
    gerarVizinha(s_vizinha);
    heuBLPM(s);
    if (s_vizinha.cost < s.cost)
    {
      memcpy(&s, &s_vizinha, sizeof(s_vizinha));
      hF = clock();
      tempo_melhor = ((double)(hF - hI)) / CLOCKS_PER_SEC;
    }
    hF = clock();
    tempo_total = ((double)(hF - hI)) / CLOCKS_PER_SEC;
    i++;
  }
  s.tempo_melhor = tempo_melhor;
  s.tempo_medio = tempo_total / i;
  s.tempo_total = tempo_total;
  //printSolution(s);
}

void heuBLPM(Solucao &s)
{
  int aux;
  int melFO = s.cost;
  for (int i = 0; i < s.trucks; i++)
  {
    for (int j = 0; s.rotas[i][j + 1] != -1; j++)
    {
      for (int y = i + 1; y < s.trucks; y++)
      {
        if (s.rotas[y][j + 1] != -1)
        {
          aux = s.rotas[i][j];
          s.rotas[i][j] = s.rotas[y][j];
          s.rotas[y][j] = aux;
          calculoFO(s);
          if (melFO < s.cost)
          {
            s.rotas[y][j] = s.rotas[i][j];
            s.rotas[i][j] = aux;
          }
          else
          {
            melFO = s.cost;
          }
        }
      }
    }
  }
  calculoFO(s);
  //printSolution(s);
}

void gerarVizinha(Solucao &s_vizinha)
{
  int rota1, rota2, pos1, pos2, nodesRota1, nodesRota2, aux;
  rota1 = rand() % (s_vizinha.trucks);
  rota2 = rand() % (s_vizinha.trucks);
  for (nodesRota1 = 0; s_vizinha.rotas[rota1][nodesRota1] != -1; nodesRota1++)
    ;
  for (nodesRota2 = 0; s_vizinha.rotas[rota2][nodesRota2] != -1; nodesRota2++)
    ;
  pos1 = rand() % (nodesRota1);
  pos2 = rand() % (nodesRota2);
  aux = s_vizinha.rotas[rota1][pos1];
  s_vizinha.rotas[rota1][pos1] = s_vizinha.rotas[rota2][pos2];
  s_vizinha.rotas[rota2][pos2] = aux;
  calculoFO(s_vizinha);
  //printSolution(s_vizinha);
};