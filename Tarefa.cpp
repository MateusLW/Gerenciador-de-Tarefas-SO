#include "Tarefa.h"

Tarefa::Tarefa(int id, std::string cor, int ingresso, int t_execucao, int prioridade) {
    this->id = id;
    this->color = cor;
    this->tempo_ingresso = ingresso;
    this->tempo_total = t_execucao;
    this->tempo_restante = t_execucao; // No início, o restante é o total
    this->prioridade = prioridade;
}

int Tarefa::getId() const { return id; }

std::string Tarefa::getColor() const { return color; }

int Tarefa::getTempoIngresso() const { return tempo_ingresso; }

int Tarefa::getTempoRestante() const { return tempo_restante; }

int Tarefa::getPrioridade() const { return prioridade; }

void Tarefa::executarUnidade() {
    if (tempo_restante > 0) {
        tempo_restante--;
    }
}

bool Tarefa::isConcluida() const {
    return tempo_restante <= 0;
}