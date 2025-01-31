#ifndef INSTRUCTION_TRANSLATOR_H_INCLUDED
#define INSTRUCTION_TRANSLATOR_H_INCLUDED

#include <bits/stdc++.h>
#include <fstream>
#include <regex>
#include <stdexcept>
#include <cctype>
#include <string>
#include <iostream>
#include "resources.h"

using namespace std;

// Mapeamento de opcodes para instruções assembly
map<string, int> instructionOpcodes = {
    {"ADD", 1}, {"SUB", 2}, {"MULT", 3}, {"DIV", 4}, {"JMP", 5}, {"JMPN", 6}, {"JMPP", 7}, {"JMPZ", 8}, {"COPY", 9}, {"LOAD", 10}, {"STORE", 11}, {"INPUT", 12}, {"OUTPUT", 13}, {"STOP", 14}};

// Lista de diretivas suportadas
unordered_set<string> assemblerDirectives = {"CONST", "EQU", "IF", "SPACE", "SECTION", "MACRO", "ENDMACRO"};

// Processa diretivas EQU
void processEquDirectives(vector<vector<string>> &program)
{
  map<string, string> equMappings;
  vector<vector<string>> processedProgram;

  // Primeira passagem: Armazenar valores EQU
  for (const auto &line : program)
  {
    if (line.size() > 1 && line[1] == "EQU")
    {
      equMappings[extractLabel(line[0])] = line[2];
    }
    else
    {
      processedProgram.push_back(line);
    }
  }

  // Segunda passagem: Substituir os valores EQU nas instruções
  for (auto &line : processedProgram)
  {
    for (auto &token : line)
    {
      while (equMappings.count(token))
      {
        token = equMappings[token];
      }
    }
  }

  program = processedProgram;
}

// Processa diretivas IF
void processIfDirectives(vector<vector<string>> &program)
{
  vector<vector<string>> processedProgram;

  for (size_t i = 0; i < program.size(); ++i)
  {
    if (program[i].size() > 1 && program[i][0] == "IF")
    {
      int condition = stoi(program[i][1]);
      if (condition == 0 && i + 1 < program.size())
      {
        ++i; // Pula a próxima linha se a condição for 0
      }
      continue; // Sempre pula a linha IF
    }
    processedProgram.push_back(program[i]);
  }

  program = processedProgram;
}

// Processas as diretivas EQU e IF
void preprocess(vector<vector<string>> &program)
{
  processEquDirectives(program);
  processIfDirectives(program);
}

// Processa macros com argumentos e expande seu conteúdo
void expandMacro(vector<vector<string>> &program, size_t &i, map<string, vector<vector<string>>> &macros)
{
  vector<vector<string>> expandedMacro;
  string macroName = program[i][0];

  if (!inMACRO(macroName, macros))
    return; // Evita loop infinito

  vector<vector<string>> macroLines = macros[macroName];
  program.erase(program.begin() + i);
  program.insert(program.begin() + i, macroLines.begin(), macroLines.end());
}

void processMACRO(vector<vector<string>> &program)
{
  map<string, vector<vector<string>>> macros;
  bool insideMacro = false;
  string currentMacro;
  vector<vector<string>> processedProgram;

  for (size_t i = 0; i < program.size(); ++i)
  {
    auto &line = program[i];

    if (insideMacro)
    {
      if (line[0] == "ENDMACRO")
      {
        insideMacro = false;
      }
      else
      {
        macros[currentMacro].push_back(line);
      }
    }
    else
    {
      if (line.size() > 1 && line[1] == "MACRO")
      {
        insideMacro = true;
        currentMacro = extractLabel(line[0]);
        macros[currentMacro] = {};
        continue;
      }

      while (inMACRO(line[0], macros))
      {
        expandMacro(program, i, macros);
        line = program[i]; // Atualiza a linha corretamente para evitar loop infinito
      }
      processedProgram.push_back(line);
    }
  }
  program = processedProgram;
}

// Processa a primeira passagem do montador usando tokens
map<string, int> passOne(const vector<vector<string>> &program)
{
  map<string, int> symbolTable;
  int addressCounter = 0;

  for (const auto &line : program)
  {
    vector<string> tokens = line;
    if (!tokens.empty() && isLabel(tokens[0]))
    {
      string label = extractLabel(tokens[0]);
      if (!isalpha(label[0]))
      {
        throw invalid_argument("\nErro Léxico: Rótulo inválido na linha " + to_string(addressCounter) + ": " + label + ".\n");
      }
      if (symbolTable.count(label))
      {
        throw invalid_argument("\nErro Semântico: Rótulo duplicado na linha " + to_string(addressCounter) + ": " + label + ".\n");
      }
      symbolTable[label] = addressCounter;
      tokens.erase(tokens.begin()); // Remove o rótulo da linha
    }

    if (!tokens.empty())
    {
      if (isLabel(tokens[0]))
      {
        throw invalid_argument("Erro Sintático: Dois rótulos consecutivos detectados na linha " + to_string(addressCounter) + ".\n");
      }
      string op = tokens[0];
      if (instructionOpcodes.count(op))
      {
        addressCounter += (op == "COPY") ? 3 : (op == "STOP") ? 1
                                                              : 2;
      }
      else if (find(assemblerDirectives.begin(), assemblerDirectives.end(), op) != assemblerDirectives.end())
      {
        if (op == "CONST")
        {
          addressCounter += 1;
        }
        else if (op == "SPACE")
        {
          addressCounter += (tokens.size() > 1) ? stoi(tokens[1]) : 1;
        }
      }
      else
      {
        throw invalid_argument("Erro Semântico: Operação desconhecida na linha " + to_string(addressCounter) + ".");
      }
    }
  }
  return symbolTable;
}

string passTwo(vector<vector<string>> &program, map<string, int> &symbolTable)
{
  int addressCounter = 0;
  int lineCounter = 1;
  string objectCode;
  bool hasTEXT = false;

  // Printa a tabela de símbolos
  // cout << "Tabela de Símbolos:" << endl;
  // for (const auto &entry : symbolTable)
  // {
  //   cout << entry.first << " -> " << entry.second << endl;
  // }

  for (const auto &line : program)
  {
    string label, operation, operandsStr;
    vector<string> operands;

    // Identifica rótulo e operação
    if (isLabel(line[0]))
    {
      label = extractLabel(line[0]);
      operation = line.size() > 1 ? line[1] : "";
      operandsStr = line.size() > 2 ? line[2] : "";
    }
    else
    {
      operation = line[0];
      operandsStr = line.size() > 1 ? line[1] : "";
    }

    if (operation != "SECTION")
    {
      operands = splitOperands(operandsStr);

      // Verifica se os operandos estão na tabela de símbolos
      for (const auto &operand : operands)
      {
        if (isSymbol(operand) && !isInSymbolTable(operand, symbolTable))
        {

          throw invalid_argument("Erro Semântico: Uso de dado não declarado na linha " + to_string(lineCounter) + ".");
        }
      }

      if (instructionOpcodes.count(operation))
      {
        // Verificação de número de argumentos
        if ((operation == "COPY" && operands.size() != 2) ||
            (operation == "STOP" && operands.size() != 0) ||
            (operation != "COPY" && operation != "STOP" && operands.size() != 1))
        {
          throw invalid_argument("Erro Sintático: Número errado de argumentos na linha " + to_string(lineCounter) + ".");
        }

        objectCode += to_string(instructionOpcodes[operation]);
        for (const auto &operand : operands)
        {
          int offset = (line.size() > 3) ? stoi(line.back()) : 0;
          objectCode += " " + to_string(symbolTable[operand] + offset);
        }
        objectCode += " ";
        addressCounter += 1 + operands.size();
      }
      else if (find(assemblerDirectives.begin(), assemblerDirectives.end(), operation) != assemblerDirectives.end())
      {
        if (operation == "CONST")
        {
          objectCode += operands[0] + " ";
          addressCounter += 1;
        }
        else if (operation == "SPACE")
        {
          int spaceSize = (operands.size() == 1) ? stoi(operands[0]) : 1;
          for (int i = 0; i < spaceSize; ++i)
          {
            objectCode += "0 ";
          }
          addressCounter += spaceSize;
        }
      }
      else
      {
        throw invalid_argument("Erro Semântico: Operação desconhecida na linha " + to_string(lineCounter) + ".");
      }
    }
    else
    {
      if (line.size() > 1 && line[1] == "TEXT")
      {
        hasTEXT = true;
      }
    }
    lineCounter++;
  }

  if (!hasTEXT)
  {
    throw invalid_argument("Erro Semântico: SECTION TEXT ausente.");
  }

  return objectCode;
}

#endif // INSTRUCTION_TRANSLATOR_H_INCLUDED
