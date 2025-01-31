#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <string>

#include "instruction_translator.h"
#include "token_parser.h"
#include "resources.h"

using namespace std;

void executePreprocessing(const string &inputFile)
{
  vector<vector<string>> parsedData = parseTokens(inputFile);
  preprocess(parsedData);
  saveToFile(parsedData, inputFile, ".PRE");
}

void executeMacroProcessing(const string &inputFile)
{
  vector<vector<string>> parsedData = parseTokens(inputFile);
  preprocess(parsedData);
  processMACRO(parsedData);
  saveToFile(parsedData, inputFile, ".MCR");
}

void executeObjectGeneration(const string &inputFile)
{
  vector<vector<string>> parsedData = parseTokens(inputFile);
  preprocess(parsedData);
  processMACRO(parsedData);
  map<string, int> symbolTable = passOne(parsedData);
  string objectCode = passTwo(parsedData, symbolTable);
  saveObjectFile(objectCode, inputFile, ".OBJ");
}

void executeAll(const string &inputFile)
{
  vector<vector<string>> parsedData = parseTokens(inputFile);
  saveToFile(parsedData, inputFile, ".PSRD");
  preprocess(parsedData);
  saveToFile(parsedData, inputFile, ".PRE");
  processMACRO(parsedData);
  saveToFile(parsedData, inputFile, ".MCR");
  map<string, int> symbolTable = passOne(parsedData);
  string objectCode = passTwo(parsedData, symbolTable);
  saveObjectFile(objectCode, inputFile, ".OBJ");
}

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    cerr << "Uso: " << argv[0] << " <opção> <arquivo>" << endl;
    return 1;
  }

  string operation = argv[1];
  string inputFile = argv[2];

  if (!filesystem::exists(inputFile))
  {
    cerr << "Erro: O arquivo de entrada " << inputFile << " não existe." << endl;
    return 1;
  }

  if (operation == "-p")
  {
    executePreprocessing(inputFile);
  }
  else if (operation == "-m")
  {
    executeMacroProcessing(inputFile);
  }
  else if (operation == "-o")
  {
    executeObjectGeneration(inputFile);
  }
  else if (operation == "-a")
  {
    executeAll(inputFile);
  }
  else
  {
    cerr << "Opção inválida! Use -p, -m, -o, -a." << endl;
    return 1;
  }

  return 0;
}