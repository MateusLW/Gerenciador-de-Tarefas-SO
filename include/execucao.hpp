#pragma once
#include <vector>
#include <string>
#include <memory>
#include "cpu.hpp"
#include "escalonador.hpp"

namespace ProjetoSO 
{
	class Execucao 
	{
		private:
			std::string type;
			std::unique_ptr<Escalonador> escalonador;
			std::vector<CPU*> cpu_list;
			std::vector<Tarefa*> tarefas;
			std::vector<Tarefa*> todas_tarefas;
			std::vector<Tarefa*> finalizadas;
			unsigned int quantum;
			int relogio;

			void entra_tarefa();
			void quantum_tempo();
			std::unique_ptr<Escalonador> criarEscalonador(const std::string& type, unsigned int quantum);

		public:

			Execucao() = default;
			Execucao(const Execucao&) = delete;
			void operator=(const Execucao&) = delete;

			static Execucao* getInstance();
			void init(std::string type, std::vector<Tarefa*> todas_tarefas, unsigned int quantum, unsigned int cpuCount);
			void finalizar_tarefa(Tarefa* t);
			void mandar_fila_prontos(Tarefa* t);
			void mandar_cpu(Tarefa* t);
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