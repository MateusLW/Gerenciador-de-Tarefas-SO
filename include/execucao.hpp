#pragma once
#include <vector>
#include <string>
#include "cpu.hpp"

namespace ProjetoSO 
{
	class Execucao 
	{
		private:
			static Execucao instance;

			std::string type;
			std::vector<CPU*> cpu_list;
			std::vector<Tarefa*> tarefas;
			std::vector<Tarefa*> todas_tarefas;
			std::vector<Tarefa*> finalizadas;
			unsigned int quantum;
			int relogio;

			void entra_tarefa();
			void SRTF(std::vector<CPU*> cpu_quantum);
			void PRIOP();
			void quantum_tempo();

		public:

			Execucao() = default;
			Execucao(const Execucao&) = delete;
			void operator=(const Execucao&) = delete;

			static Execucao* getInstance();
			void init(std::string type, std::vector<Tarefa*> todas_tarefas, unsigned int quantum, unsigned int cpuCount);
			void finalizar_tarefa(Tarefa* t);
			void mandar_fila_prontos(Tarefa* t);
			void mandar_cpu(Tarefa* t);
			Tarefa* sorteio(const std::vector<Tarefa*>& empates);
			bool update();
			void unidade_tempo();
			void automatico();

			std::vector<CPU*> getCpuList() const {return cpu_list;}
			std::vector<Tarefa*> getTarefas() const {return tarefas;}
			std::vector<Tarefa*> getFinalizadas() const {return finalizadas;}
			int getRelogio() const {return relogio;}
			bool getCPUVazio();
	};
}