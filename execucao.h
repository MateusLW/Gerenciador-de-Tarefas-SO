#ifndef EXECUCAO_H
#define EXECUCAO_H

#include <string>
#include <vector>
#include "Tarefa.h" // Ajuste o caminho conforme sua pasta
#include "CPU.h"
using namespace std;

class CPU;
class Tarefa;

class execucao {
private:
    std::string type;
    std::vector<CPU*> cpu_list;
    std::vector<Tarefa*> tarefas;
    int quantum;
    int relogio = 0; // Não esqueça de declarar o relógio

    void entra_tarefa();
    void SRTF();
    void quantum_tempo();

public:
    execucao(std::string type, std::vector<CPU*> cpu_list, int quantum);
    void execute();
};

#endif