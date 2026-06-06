#pragma once
#include "raylib.h"
#include <vector>

namespace ProjetoSO
{
	class Tarefa
	{
		public:
			enum class TaskState
			{
				Idle,
				Ready,
				Executing,
				Finished
			};

			struct Event
			{
				TaskState state;
				unsigned int begin;
				unsigned int end;
			};

		private:
			int id;
			Color color;
			int tempo_ingresso;
			int tempo_total;      // Tempo total que a tarefa precisa (lido do arquivo)
			int tempo_restante;   // Quanto ainda falta (essencial para SRTF)
			int prioridade;
			std::vector<Event> events;

		public:
		
			// Construtor atualizado
			Tarefa(int id, Color cor, int ingresso, int t_execucao, int prioridade);
			// Getters
			Color getColor() const { return color; }
			int getTempoIngresso() const { return tempo_ingresso; }
			int getTempoTotal() const { return tempo_total; }
			int getTempoRestante() const { return tempo_restante; }
			int getPrioridade() const { return prioridade; }
			int getId() const { return id; }
			std::vector<Event> getEvents() const { return events; }
			

			// Métodos de Processamento
			void executarUnidade(); // Reduz o tempo restante
			bool isConcluida() const;
			void registrarEvento(TaskState state, unsigned int relogio);
			void resetEventos() { events.clear(); }
		
	};
}