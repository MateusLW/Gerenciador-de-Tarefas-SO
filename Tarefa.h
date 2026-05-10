#ifndef TAREFA_H
#define TAREFA_H

#include <string>
using namespace std;

class Tarefa {
private:
    int id;
    std::string color;
    int tempo_ingresso;
    int tempo_total;      // Tempo total que a tarefa precisa (lido do arquivo)
    int tempo_restante;   // Quanto ainda falta (essencial para SRTF)
    int prioridade;

public:
    // Construtor atualizado
    Tarefa(int id, std::string cor, int ingresso, int t_execucao, int prioridade);

    // Getters
    int getId() const;
    std::string getColor() const;
    int getTempoIngresso() const;
    int getTempoRestante() const;
    int getPrioridade() const;

    // Métodos de Processamento
    void executarUnidade(); // Reduz o tempo restante
    bool isConcluida() const;
};

#endif