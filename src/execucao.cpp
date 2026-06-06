#include "../include/execucao.hpp"

#include <iostream>
#include <string>
#include <algorithm>

using namespace ProjetoSO;
using namespace std;

Execucao* Execucao::getInstance()
{
	static Execucao instance;
	return &instance;
}

void Execucao::init(std::string type, std::vector<Tarefa*> todas_tarefas, unsigned int quantum, unsigned int cpuCount)
{
    if (cpu_list.size() > 0)
		for (CPU* cpu : cpu_list)
			delete cpu;
	this->cpu_list.clear();
	this->todas_tarefas.clear();
	this->type = type;
	this->todas_tarefas = todas_tarefas;
	this->quantum = quantum;
	tarefas.clear();
	for (int i = 0; i < cpuCount; i++)
		cpu_list.push_back(new CPU(i));
	relogio = 0;
	for (Tarefa* t : todas_tarefas) {
		t->resetEventos();
	}
	cout << ">> Execucao inicializada com " << cpuCount << " CPUs, quantum de " << quantum << "s, tipo de escalonamento " << type << " e " << todas_tarefas.size() << " tarefas." << endl;
}

void Execucao::entra_tarefa() 
{
	for (Tarefa* t : todas_tarefas) {
		if (t->getTempoIngresso() == relogio) {
			tarefas.push_back(t);
			t->registrarEvento(Tarefa::TaskState::Ready, relogio);
		}
	}
}

void Execucao::finalizar_tarefa(Tarefa* t) 
{

    if (std::any_of(cpu_list.begin(), cpu_list.end(), [t](CPU* c){ return c->getTarefa() == t; })) {
        for (CPU* c : cpu_list) {
            if (c->getTarefa() == t) {
                c->setTarefa(nullptr);
            }
        }
    }
    else if (std::any_of(tarefas.begin(), tarefas.end(), [t](Tarefa* tarefa){ return tarefa == t; })) {
        for (int i=0; i<tarefas.size(); i++) {
            if (t == tarefas[i]) {
                tarefas.erase(tarefas.begin() + i);
            }
        }
    }

}

void Execucao::mandar_fila_prontos(Tarefa* t){

    for (CPU* cpu : cpu_list) {
        if (cpu->getTarefa() == t) {
            cpu->setTarefa(nullptr);
            tarefas.push_back(t);
			break;
        }
    }
}

void Execucao::mandar_cpu(Tarefa* t) {
    int index_menor = 0;
    CPU* cpu_menor;
    for (CPU* cpu : cpu_list) {
        if (cpu->getTarefa() == nullptr) {
            cpu->setTarefa(t);
            break;
        }
    }
}

bool Execucao::getCPUVazio() {
	for (CPU* cpu : cpu_list) {
		if (!cpu->isOcupado()) return true;
	}
	return false;
}

void Execucao::automatico() 
{
	while (!update()) {	}
}

bool Execucao::update() 
{
    entra_tarefa();
	unidade_tempo();

    bool terminou = false;
    if (type == "SRTF") {

        std::vector<CPU*> cpu_quantum;
        for (CPU* cpu : cpu_list) {
            if (cpu->getTempo_exec()%quantum == 0 || getCPUVazio())
                cpu_quantum.push_back(cpu);
        }
        SRTF(cpu_quantum);

        terminou = tarefas.empty();
        for (CPU* cpu : cpu_list) {
            if (cpu->getTarefa() != nullptr) terminou = false;
        }
    }
    else if (type == "PRIOP") {

        PRIOP();

        terminou = tarefas.empty();
        for (CPU* cpu : cpu_list) {
            if (cpu->getTarefa() != nullptr) terminou = false;
        }
    }
    else
        return true;
	relogio++;
    for (CPU* cpu: cpu_list)
        cpu->executando();

    return terminou;
}

void Execucao::unidade_tempo() 
{
    for (CPU* cpu : cpu_list) {
        Tarefa* t = cpu->getTarefa();

        //Tarefa em processador executa
        if (t != nullptr) {
            t->executarUnidade();

            //Libera CPU caso seja concluida
            if (t->isConcluida())
			{
                cpu->liberarCPU();
				t->registrarEvento(Tarefa::TaskState::Finished, relogio);
				finalizadas.push_back(t);
			}

        } //Processador sem executar nenhuma tarefa
        else {
            cpu->unidadeDesligado();
        }
    }
}

void Execucao::SRTF(std::vector<CPU*> cpu_quantum) 
{
    for (int n = 0; n < cpu_quantum.size(); n++) {
        if (tarefas.empty()) break;
        vector<Tarefa*> empates;
        //Procura tarefa com menor tempo de execução restante
        int idx_fila_menor = 0;
        for (int i = 1; i < tarefas.size(); i++) {
            //Condições de desempate
            if (tarefas[i]->getTempoRestante() == tarefas[idx_fila_menor]->getTempoRestante()) {
                if (tarefas[i]->getTempoIngresso() == tarefas[idx_fila_menor]->getTempoIngresso()) {
                    empates.push_back(tarefas[i]);
                }
                else if (tarefas[i]->getTempoIngresso() < tarefas[idx_fila_menor]->getTempoIngresso()) {
                    empates.clear();
                    idx_fila_menor = i;
                    empates.push_back(tarefas[i]);
                }
            }
            else if (tarefas[i]->getTempoRestante() < tarefas[idx_fila_menor]->getTempoRestante()){
                empates.clear();
                idx_fila_menor = i;
                empates.push_back(tarefas[i]);
            }

        }
        Tarefa* melhor_da_fila = tarefas[idx_fila_menor];

        //Procura processador com a tarefa de maior tempo restante ou nulo
        CPU* cpu_alvo = nullptr;
        int maior_tempo_execucao = -1;
        bool encontrou_nulo = false;
        for (CPU* cpu : cpu_list) {
            if (cpu->getTarefa() == nullptr) {
                cpu_alvo = cpu;
                encontrou_nulo = true;
                break;
            }
            else {
                int tempo_cpu = cpu->getTarefa()->getTempoRestante();
                if (tempo_cpu > maior_tempo_execucao) {
                    maior_tempo_execucao = tempo_cpu;
                    cpu_alvo = cpu;
                }
            }
        }

        //Processador sem tarefa executando
        if (encontrou_nulo) {
            if (empates.size() > 1) {
                melhor_da_fila = sorteio(empates); // <-- Função de sorteio (Retorna Tarefa*)
            }
            cpu_alvo->setTarefa(melhor_da_fila);
            tarefas.erase(tarefas.begin() + idx_fila_menor);
			cpu_alvo->getTarefa()->registrarEvento(Tarefa::TaskState::Executing, relogio);
            cpu_alvo->resetExec();
        }

        //Processador com tarefa de maior tempo restante comparado ao de menor tempo da fila de prontos
        else if (cpu_alvo != nullptr && melhor_da_fila->getTempoRestante() < cpu_alvo->getTarefa()->getTempoRestante()) {
            if (empates.size() > 1) {
                melhor_da_fila = sorteio(empates); // <-- Função de sorteio (Retorna Tarefa*)
            }
            Tarefa* saindo = cpu_alvo->getTarefa();

            cpu_alvo->setTarefa(melhor_da_fila);
            tarefas.erase(tarefas.begin() + idx_fila_menor);
			cpu_alvo->getTarefa()->registrarEvento(Tarefa::TaskState::Executing, relogio);
			saindo->registrarEvento(saindo->isConcluida() ? Tarefa::TaskState::Finished : Tarefa::TaskState::Ready, relogio);
            tarefas.push_back(saindo);
            cpu_alvo->resetExec();
        }
        //Nenhuma tarefa na fila de prontos é menor que qualquer tarefa nos processadores
        else {
            break;
        }
    }
}

Tarefa* Execucao::sorteio(const vector<Tarefa*>& empates) 
{
	if (empates.empty()) return nullptr;
	int idx_sorteado = rand() % empates.size();
	return empates[idx_sorteado];
}

void Execucao::PRIOP() 
{
    for (int n = 0; n < cpu_list.size(); n++) {
        if (tarefas.empty()) break;

        //Procura tarefa com maior prioridade da fila
        int idx_fila_maior = 0;
        vector<Tarefa*> empates;
        for (int i = 1; i < tarefas.size(); i++) {
            //Condições de desempate
            if (tarefas[i]->getPrioridade() == tarefas[idx_fila_maior]->getPrioridade()) {
                if (tarefas[i]->getTempoIngresso() == tarefas[idx_fila_maior]->getTempoIngresso()) {
                    if (tarefas[i]->getTempoRestante() == tarefas[idx_fila_maior]->getTempoRestante()) {
                        empates.push_back(tarefas[i]);
                    }
                    else if (tarefas[i]->getTempoRestante() < tarefas[idx_fila_maior]->getTempoRestante()) {
                        empates.clear();
                        idx_fila_maior = i;
                        empates.push_back(tarefas[i]);
                    }
                }
                else if (tarefas[i]->getTempoIngresso() < tarefas[idx_fila_maior]->getTempoIngresso()) {
                    empates.clear();
                    idx_fila_maior = i;
                    empates.push_back(tarefas[i]);
                }
            }
            else if (tarefas[i]->getPrioridade() > tarefas[idx_fila_maior]->getPrioridade()){
                empates.clear();
                idx_fila_maior = i;
                empates.push_back(tarefas[i]);
            }
        }
        Tarefa* melhor_da_fila = tarefas[idx_fila_maior];

        //Procura processador com a tarefa de menor prioridade ou nulo
        CPU* cpu_alvo = nullptr;
        int menor_prioridade = INT_MAX;
        bool encontrou_nulo = false;
        for (CPU* cpu : cpu_list) {
            if (cpu->getTarefa() == nullptr) {
                cpu_alvo = cpu;
                encontrou_nulo = true;
                break;
            }
            int prioridade_cpu = cpu->getTarefa()->getPrioridade();
            if (prioridade_cpu < menor_prioridade) {
                menor_prioridade = prioridade_cpu;
                cpu_alvo = cpu;
            }
        }

        //Processador sem tarefa executando
        if (encontrou_nulo) {
            if (empates.size() > 1)
                melhor_da_fila = sorteio(empates); //<-- Função de sorteio (Retorna Tarefa*)
            cpu_alvo->setTarefa(melhor_da_fila);
            tarefas.erase(tarefas.begin() + idx_fila_maior);
			cpu_alvo->getTarefa()->registrarEvento(Tarefa::TaskState::Executing, relogio);
        }

        //Processador com tarefa de menor prioridade comparado ao de maior prioridade da fila de prontos
        else if (cpu_alvo != nullptr && melhor_da_fila->getPrioridade() > cpu_alvo->getTarefa()->getPrioridade()) {
            if (empates.size() > 1)
                melhor_da_fila = sorteio(empates); // <-- Função de sorteio (Retorna Tarefa*)
            Tarefa* saindo = cpu_alvo->getTarefa();

            cpu_alvo->setTarefa(melhor_da_fila);
            tarefas.erase(tarefas.begin() + idx_fila_maior);
			cpu_alvo->getTarefa()->registrarEvento(Tarefa::TaskState::Executing, relogio);
			saindo->registrarEvento(saindo->isConcluida() ? Tarefa::TaskState::Finished : Tarefa::TaskState::Ready, relogio);
            tarefas.push_back(saindo);
        }
        //Nenhuma tarefa na fila de prontos é menor que qualquer tarefa nos processadores
        else {
            break;
        }
    }
}

void Execucao::quantum_tempo()
{
    for (int i = 0; i < quantum; i++)
    {
        relogio++;
        cout << "[RELOGIO: " << relogio << "s]" << endl;
        entra_tarefa();
        for (CPU *cpu : cpu_list)
        {
            Tarefa *t = cpu->getTarefa();

            // Tarefa em processador executa
            if (t != nullptr)
            {
                t->executarUnidade();
                cout << "  [CPU " << cpu->getId() << "] Executando Tarefa " << t->getId()
                     << " | Restante: " << t->getTempoRestante() << "s" << endl;

                // Libera CPU caso seja concluida
                if (t->isConcluida())
                {
                    cout << "  [TERMINOU] Tarefa " << t->getId() << " concluida na CPU " << cpu->getId() << endl;
                    cpu->liberarCPU();
                }
                // Processador sem executar nenhuma tarefa
            }
            else
            {
                cpu->unidadeDesligado();
            }
        }
    }
}