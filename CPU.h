#ifndef CPU_H
#define CPU_H
#include "Tarefa.h"
#include <cstddef>
using namespace std;

// Avisamos ao compilador que 'Tarefa' existe, sem precisar ler o arquivo dela agora
class Tarefa;

class CPU {
private:
    int id;
    Tarefa* t;
    bool ocupado;
    int tempo_desligado;

public:
    // O construtor geralmente inicializa a CPU vazia (tarefa = nullptr)
    CPU(int id);

    // Getters
    int getId() const;
    Tarefa* getTarefa() const;
    bool isOcupado() const;
    int getTempo_desligado() const;

    // Setters e Métodos de Controle
    void setTarefa(Tarefa* nova_tarefa);
    void liberarCPU();
    void unidadeDesligado();
};

#endif