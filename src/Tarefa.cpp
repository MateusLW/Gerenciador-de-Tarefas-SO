#include "../include/Tarefa.hpp"

using namespace ProjetoSO;

Tarefa::Tarefa(int id, Color cor, int ingresso, int t_execucao, int prioridade)
{
    this->id = id;
    this->color = std::move(cor);
    this->tempo_ingresso = ingresso;
    this->tempo_total = t_execucao;
    this->tempo_restante = t_execucao; // No início, o restante é o total
    this->prioridade = prioridade;
}

void Tarefa::executarUnidade()
{
    if (tempo_restante > 0)
    {
        tempo_restante--;
    }
}

bool Tarefa::isConcluida() const
{
    return tempo_restante <= 0;
}

void Tarefa::registrarEvento(TaskState state, unsigned int relogio) 
{
	if (events.empty() || events.back().state != state) {
		if (!events.empty())events.back().end = relogio;
		Event newEvent;
		newEvent.state = state;
		newEvent.begin = relogio;
		newEvent.end = relogio;
		events.push_back(newEvent);
	} else {
		events.back().end = relogio;
	}
}