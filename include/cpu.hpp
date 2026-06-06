#pragma once

#include "tarefa.hpp"

namespace ProjetoSO
{
	class Tarefa;

	class CPU
	{
	private:
		int id;
		Tarefa *t;
		bool ocupado;
		int tempo_desligado;
		int tempo_exec;

	public:
		CPU(int id);

		// Getters
		int getId() const;
		Tarefa *getTarefa() const;
		bool isOcupado() const;
		int getTempo_desligado() const;
		int getTempo_exec() const;

		// Setters e Métodos de Controle
		void setTarefa(Tarefa *nova_tarefa);
		void liberarCPU();
		void unidadeDesligado();
		void resetExec();
		void executando();
	};
}