#ifndef RESOURCES_H_INCLUDED
#define RESOURCES_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cctype>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

// Separa operadores de uma string usando ',' como delimitador
vector<string> splitOperands(const string &ops)
{
  vector<string> tokens;
  stringstream ss(ops);
  string temp;

  while (getline(ss, temp, ','))
  {
    tokens.push_back(temp);
  }
  return tokens;
}

// Verifica se um token é um rótulo
bool isLabel(const string &token)
{
  return !token.empty() && token.back() == ':' && isalpha(token[0]);
}

// Retorna o nome do rótulo removendo ':'
string extractLabel(const string &token)
{
  return isLabel(token) ? token.substr(0, token.size() - 1) : token;
}

// Verifica se um símbolo está na tabela de símbolos
bool isInSymbolTable(const string &token, const map<string, int> &symbolTable)
{
  return symbolTable.find(token) != symbolTable.end();
}

// Verifica se um token representa um número
bool isNumber(const string &token)
{
  return !token.empty() && all_of(token.begin(), token.end(), ::isdigit);
}

// Verifica se um token representa um símbolo válido
bool isSymbol(const string &token)
{
  if (token.empty() || isNumber(token))
    return false;
  return all_of(token.begin(), token.end(), [](char c)
                { return isalnum(c) || c == '_'; });
}

// Verifica se um nome de rótulo é válido
bool isValidLabel(const string &token)
{
  return !token.empty() && isalpha(token[0]) && all_of(token.begin(), token.end(), [](char c)
                                                       { return isalnum(c) || c == '_'; });
}

// Converte uma string para maiúsculas
string toUpperCase(string &str)
{
  transform(str.begin(), str.end(), str.begin(), ::toupper);
  return str;
}

// Remove espaços em branco extras de uma string
string trim(const string &str)
{
  size_t first = str.find_first_not_of(" \t");
  if (first == string::npos)
    return "";
  size_t last = str.find_last_not_of(" \t");
  return str.substr(first, last - first + 1);
}

bool isArg(const string &value, const map<string, string> &inds)
{
  return inds.find(value) != inds.end();
}

string joinOperands(const vector<string> &strs)
{
  if (strs.empty())
    return "";

  ostringstream imploded;
  for (size_t i = 0; i < strs.size(); ++i)
  {
    if (i > 0)
      imploded << ",";
    imploded << strs[i];
  }

  return imploded.str();
}

bool inMACRO(const string &token, const map<string, vector<vector<string>>> &macros)
{
  return macros.find(token) != macros.end();
}

int getValueMACRO(string token)
{
  if (!token.empty() && token[0] == '#')
  {
    token.erase(0, 1);
  }
  return stoi(token);
}

string changeExtension(const string &fileName, const string &newExtension) {
    size_t dotPos = fileName.find_last_of(".");
    if (dotPos != string::npos) {
        return fileName.substr(0, dotPos) + newExtension;
    }
    return fileName + newExtension;
}

void saveToFile(const vector<vector<string>> &program, const string &fileName, const string &newExtension) {
    string outputFileName = changeExtension(fileName, newExtension);
    ofstream outFile(outputFileName);
    if (!outFile) {
        cerr << "Erro ao abrir o arquivo: " << outputFileName << endl;
        return;
    }
    for (const auto &line : program) {
        for (const auto &token : line) {
            outFile << token << " ";
        }
        outFile << "\n";
    }
    outFile.close();
}

void saveObjectFile(const string &objectCode, const string &fileName, const string &newExtension) {
    string outputFileName = changeExtension(fileName, newExtension);
    ofstream outFile(outputFileName);
    if (!outFile) {
        cerr << "Erro ao abrir o arquivo: " << outputFileName << endl;
        return;
    }
    outFile << objectCode;
    outFile.close();
}

#endif // RESOURCES_H_INCLUDED
