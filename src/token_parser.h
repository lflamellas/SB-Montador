#ifndef TOKEN_PARSER_H_INCLUDED
#define TOKEN_PARSER_H_INCLUDED

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include "instruction_translator.h"

using namespace std;

// Divide um token contendo '+' em partes separadas
vector<string> splitByPlus(const string &token)
{
  vector<string> result;
  istringstream stream(token);
  string segment;
  while (getline(stream, segment, '+'))
  {
    result.push_back(segment);
  }
  return result;
}

// Lê um arquivo e divide seu conteúdo em tokens organizados por linha
vector<vector<string>> parseTokens(const string &filename)
{
  ifstream file(filename);
  string lineContent;
  string hex_prefix = "0X";
  vector<vector<string>> parsedProgram;
  vector<string> lineTokens;
  vector<string> tmp;

  while (getline(file, lineContent))
  {
    lineContent = trim(toUpperCase(lineContent));
    istringstream lineStream(lineContent.substr(0, lineContent.find(';')));
    string token;

    lineTokens.clear();
    while (lineStream >> token)
    {
      if (token.rfind(hex_prefix, 0) == 0)
      {
        token = to_string(stoul(token.substr(hex_prefix.length()), nullptr, 16));
      }

      if (token.find('+') != string::npos && token.size() > 1)
      {
        tmp = splitByPlus(token);
        lineTokens.push_back(tmp[0]);
        lineTokens.push_back("+");
        lineTokens.push_back(tmp[1]);
      }
      else
      {
        lineTokens.push_back(token);
      }
    }

    if (!lineTokens.empty())
    {
      parsedProgram.push_back(lineTokens);
    }
  }

  return parsedProgram;
}

#endif // TOKEN_PARSER_H_INCLUDED
