#include <iostream>
#include <string>
#include <dirent.h>
#include <fstream>
#include <list>

using namespace std;

#define SIZE 101

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

void getIndexesNodePositions(int positions[], string line)
{
  int spaceOrder = 0;
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

void printInstace(Instance instance){
  cout << "name: " << instance.name << " dimension: " << instance.dimension << " trucks: " << instance.trucks << endl;
  for (int i = 0; i < instance.dimension; i++){
    cout << "node: " << instance.x[i] << " " << instance.y[i] << " " << instance.demand[i] << endl;
  }
}

void getDimensionAndTrucksFromName(string name, int *dimension, int *instanceTrucks);
string getFileToRead();
string current_working_directory();
void readInstance(string directory, Instance *instance);

int main(int argc, char *argv[])
{
  Instance instance;
  string directoryFiles = current_working_directory() + "\\";

  instance.name = getFileToRead();
  directoryFiles += instance.name + "\\";
  getDimensionAndTrucksFromName(instance.name, &instance.dimension, &instance.trucks);
  readInstance(directoryFiles, &instance);
  printInstace(instance);
  return 0;
}

void getDimensionAndTrucksFromName(string name, int *dimension, int *instanceTrucks){
  int stratDimension;
  for (int i = 0; i < name.size(); i++)
  {
    if (name[i] == 'n')
    {
      stratDimension = i+1;
    }
    if (name[i] == 'k')
    {
      *dimension = stoi(name.substr(stratDimension,i));
      *instanceTrucks = stoi(name.substr(i+1,name.size()));
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
  char *cwd = _getcwd(0, 0);
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
