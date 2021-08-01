#include <iostream>
#include <string>
#include <dirent.h>
#include <fstream>
#include <list>
#include <unistd.h>
#include <math.h>

using namespace std;

#define SIZE 101
#define ROTAS 14
#define NODE 101

class Instance
{
public:
  string name;
  int dimension;
  int capacity;
  int trucks;
  int x[SIZE];
  int y[SIZE];
  int demand[SIZE];
};

class Solucao
{
public:
  int cost;
  int trucks;
  int rotas[ROTAS][NODE];
};

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
    for (int j = 0; solution.rotas[i][j] != NULL; j++)
    {
      cout << solution.rotas[i][j] << " ";
    }
  }
  cout << "\nCost " << solution.cost << endl;
}

void getDimensionAndTrucksFromName(string name, int *dimension, int *instanceTrucks);
string getFileToRead();
string current_working_directory();
void readInstance(string directory, Instance *instance);
void heuConAle(Solucao &s, Instance inst);
int calcDistancia(int x1, int x2, int y1, int y2);
int jaAtendido(int atendidos[], int atual);

int main(int argc, char *argv[])
{
  Instance instance;
  Solucao solucao;
  // DESCOMENTAR SE RODAR NO WINDOWS
  //string directoryFiles = current_working_directory() + "\\";
  // COMENTAR SE RODAR NO WINDOWS
  string directoryFiles = current_working_directory() + "/";

  instance.name = getFileToRead();
  // DESCOMENTAR SE RODAR NO WINDOWS
  //directoryFiles += instance.name + "\\";
  // COMENTAR SE RODAR NO WINDOWS
  directoryFiles += instance.name + "/";
  getDimensionAndTrucksFromName(instance.name, &instance.dimension, &instance.trucks);
  readInstance(directoryFiles, &instance);
  //printInstace(instance);
  heuConAle(solucao, instance);
  printSolution(solucao);
  return 0;
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
      if (line.find("NODE_COORD_SECTION") == 0)
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
          getIndexesNodePositions(positions, line);
          instance->x[index] = stoi(line.substr(positions[0], positions[1] - 1));
          instance->y[index] = stoi(line.substr(positions[1], line.length()));
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
          index++;
        }
      }
    }
    file.close();
  }
}

void heuConAle(Solucao &s, Instance inst)
{
  s.trucks = inst.trucks;
  int rota = 0;
  int capacityTruk = 0;
  int posiRota = 0;
  s.cost = 0;
  int atendidos[32]; 
  atendidos[0] = 1; 
  for (int i = 1; i < inst.dimension; i++)
  {
    int node = (rand() % (inst.dimension + 1));

    for (i = 0; i < inst.dimension; i++)
    {
      /* code */
    }

    if (capacityTruk + inst.demand[node] <= 100)
    {
      s.rotas[rota][posiRota] = node;
      s.cost += calcDistancia(inst.x[s.rotas[rota][posiRota - 1]], inst.x[node], inst.y[s.rotas[rota][posiRota - 1]], inst.y[node]);
      capacityTruk += inst.demand[node];
      posiRota++;
    }
    else
    {
      s.rotas[rota][posiRota + 1] = NULL;
      rota++;
      posiRota = 0;
      capacityTruk = 0;
    }
  }

  int x = 0;
}

// void heuConAle(Solucao &s, Instance inst)
// {
//   int atendidos[inst.dimension - 1];
//   int totalDemand = 0;
//   int demandPerTruck = 0;
//   for (int i = 0; i < inst.dimension; i++)
//   {
//     totalDemand += inst.demand[i];
//   }

//   demandPerTruck = totalDemand / inst.trucks;

//   int posicao = 1;
//   int custoBeneficio = -1;
//   int custo = 0;
//   int j = 1;
//   int ultimoAtendido = 0;

//   while (j < inst.dimension || custo < 100)
//   {
//     for (int i = 1; i < inst.dimension; i++)
//     {
//       if (ultimoAtendido == i)
//       {
//         continue;
//       };
//       int dist;
//       int cb;
//       if (custo == 0)
//       {
//         dist = calcDistancia(inst.x[0], inst.x[i], inst.y[0], inst.y[i]);
//       }
//       else
//       {
//         dist = calcDistancia(inst.x[posicao], inst.x[i], inst.y[posicao], inst.y[i]);
//       }

//       cb = dist / inst.demand[i];

//       if (cb > custoBeneficio)
//       {
//         posicao = i;
//         custoBeneficio = cb;
//       }

//       //cout << "1 -> " << i+1 <<" distancia: " << dist << " | custo/distancia " << dist/inst.demand[i] << endl;
//     }
//     custo += inst.demand[posicao];
//     cout << inst.demand[posicao] << endl;
//     cout << "posicao " << posicao << " c/b " << custoBeneficio << endl;
//     custoBeneficio = -1;
//     ultimoAtendido = posicao;
//     j++;
//   }
// }

int calcDistancia(int x1, int x2, int y1, int y2)
{
  return sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));
}