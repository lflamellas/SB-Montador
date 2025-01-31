# Montador Assembly - Projeto de Software Básico

## Descrição
Este projeto implementa um **montador em C++** para uma linguagem Assembly hipotética, seguindo as diretrizes da disciplina de **Software Básico**. O montador executa o processo de tradução em **três etapas principais**:
1. **Pré-processamento** - Processa diretivas IF e EQU.
2. **Expansão de MACROs** - Substitui macros definidas no código.
3. **Geração do Código-Objeto** - Converte o Assembly processado em um formato binário intermediário.

## **Especificações do Ambiente**
- **Sistema Operacional:** macOS 14.6.1
- **Compilador:** g++ 17+
- **Compilação do Montador:**
  ```sh
  g++ -std=c++17 -o montador main.cpp
  ```

## **Execução do Montador**
O montador pode ser executado de diferentes maneiras, dependendo da etapa desejada ou se todas as etapas devem ser executadas juntas.
O montador deve ser executado com uma das quatro opções disponíveis, indicando a etapa do processamento desejada.

### **Pré-Processamento**
Processa diretivas **IF** e **EQU**, gerando um arquivo com extensão `.PRE`.
```sh
./montador -p ../tests/arquivo.asm
```
**Saída:** `arquivo.PRE`

### **Processamento de MACROS**
Expande **MACROs** previamente definidas, gerando um arquivo `.MCR`.
```sh
./montador -m ../tests/arquivo.asm
```
**Saída:** `arquivo.MCR`

### **Geração do Código-Objeto**
Gera o arquivo **objeto** `.OBJ`, representando o código binário intermediário.
Gera o arquivo **objeto** `.OBJ`, representando o código binário intermediário.
```sh
./montador -o ../tests/arquivo.asm
```
**Saída:** `arquivo.OBJ`

### **Execução Completa**
Executa todas as etapas anteriores em sequência automaticamente.
```sh
./montador -a caminho/para/arquivo.asm
```
**Saída:** `arquivo.PRE` `arquivo.MCR` `arquivo.OBJ`

