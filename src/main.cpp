#ifdef _WIN32
#include <sys/stat.h>
// Define a função que o Linker está procurando, redirecionando para a versão moderna
extern "C"
{
	int stat64i32(const char *path, struct _stat64i32 *buffer)
	{
		return _stat64i32(path, buffer);
	}
}
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "../include/interface.hpp"
#include "../include/execucao.hpp"

using namespace ProjetoSO;
using namespace std;

vector<Tarefa*> pega_tarefas() {

	vector<Tarefa*> tarefas;

	//Abre arquivo
	ifstream arquivo("./tarefas.txt");
	if (!arquivo.is_open()) 
	{
		Tarefa* t1 = new Tarefa(1, {134, 206, 203, 255}, 0, 6, 5);
		Tarefa* t2 = new Tarefa(2, {190, 200, 209, 255}, 2, 4, 1);
		Tarefa* t3 = new Tarefa(3, {255, 40, 133, 255}, 0, 7, 5);
		Tarefa* t4 = new Tarefa(4, {255, 0, 69, 255}, 3, 2, 2);
		tarefas.push_back(t1);
		tarefas.push_back(t2);
		tarefas.push_back(t3);
		tarefas.push_back(t4);
		return tarefas;
	}
	cout << ">> Arquivo aberto com sucesso!" << endl;

	string linha;
	//Pula a primeira linha
	getline(arquivo, linha);
	cout << ">> Primeira linha pulada" << endl;
	
	//Percorre todas as linhas
	while (getline(arquivo, linha)) {
		if (linha.empty()) continue; // Ignora linhas em branco

		stringstream ss(linha);
		cout << ">> Processando linha: " << linha << endl;
		
		//Armazena itens separados por delimitador
		string temp;
		string hexColor;
		int id, t_ingresso, t_execucao, prioridade;
		
		getline(ss, temp, ';'); id = stoi(temp);
		getline(ss, hexColor, ';');
		getline(ss, temp, ';'); t_ingresso = stoi(temp);
		getline(ss, temp, ';'); t_execucao = stoi(temp);
		getline(ss, temp, ';'); prioridade = stoi(temp);
		cout << ">> Dados extraídos: ID=" << id << " Cor=" << hexColor << " Ingresso=" << t_ingresso << " Execucao=" << t_execucao << " Prioridade=" << prioridade << endl;

		string hexR = hexColor.substr(0,2);
		string hexG = hexColor.substr(2, 2);
		string hexB = hexColor.substr(4, 2);

		cout << ">> Componentes RGB extraídos: R=" << hexR << " G=" << hexG << " B=" << hexB << endl;

		unsigned char r = stoi(hexR, nullptr, 16);
		unsigned char g = stoi(hexG, nullptr, 16);
		unsigned char b = stoi(hexB, nullptr, 16);

		Color cor = {r, g, b, 255};
		cout << ">> Cor convertida: R=" << (int)cor.r << " G=" << (int)cor.g << " B=" << (int)cor.b << endl;

		Tarefa* nova_t = new Tarefa(id, cor, t_ingresso, t_execucao, prioridade);
		cout << ">> Tarefa criada: ID=" << nova_t->getId() << endl;

		// --- INÍCIO DA LÓGICA DE AÇÕES (E/S e Mutex) ---
		// Lê as eventuais ações após os atributos básicos da tarefa
		while (getline(ss, temp, ';')) {
			// Limpa espaços ou quebras de linha acidentais
			if (temp.empty() || temp == "\r" || temp == "\n") continue;

			stringstream ss_acao(temp);
			string tipo;
			ss_acao >> tipo; // Lê o tipo "ML", "MU" ou "IO"

			if (tipo.empty()) continue;

			Tarefa::Acao acao;
			acao.tipo = tipo;
			acao.concluida = false;

			if (tipo == "ML" || tipo == "MU") {
				ss_acao >> acao.id_mutex >> acao.instante;
				acao.duracao = 0;
				nova_t->addAcao(acao);
				cout << ">> Acao identificada: " << tipo << " | Mutex: " << acao.id_mutex << " | Instante relativo: " << acao.instante << endl;
			}
			else if (tipo == "IO") {
				ss_acao >> acao.instante >> acao.duracao;
				acao.id_mutex = -1;
				nova_t->addAcao(acao);
				cout << ">> Acao identificada: " << tipo << " | Instante relativo: " << acao.instante << " | Duracao: " << acao.duracao << endl;
			}
		}
		// --- FIM DA LÓGICA DE AÇÕES ---

		tarefas.push_back(nova_t);
	}
	
	cout << ">> Tarefas carregadas" << endl;

	return tarefas;
}

void readFile(int &quantum, int &num_cpus, string &type) 
{
    ifstream arquivo("./tarefas.txt");

    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo de configuração!" << endl;
    }
    else {

        //Checa a primeira linha
        string linha;
        if (getline(arquivo, linha)) {
            stringstream ss(linha);

            //Percorre linha pegando separando seus itens pelo demilitador
            string temp;
            getline(ss, temp, ';');
			type = temp;
            //Transforma o texto para caixa alta
			getline(ss, temp, ';'); quantum = stoi(temp);
            getline(ss, temp, ';'); num_cpus = stoi(temp);
        }
        arquivo.close();
    }
}

int main()
{
	Interface interface(1900, 1000, "Simulador de Escalonamento de Tarefas");
	SetTargetFPS(60);
	int quantum = 0, num_cpus = 0;
	string type;
	readFile(quantum, num_cpus, type);
	cout << type << " " << quantum << " " << num_cpus << endl;
	interface.setQuantum(quantum);
	interface.setCPUCount(num_cpus);
	interface.setSimulationType(type);
	interface.setTasks(pega_tarefas());
	cout << ">> Tarefas definidas na interface" << endl;

	while (!WindowShouldClose())
	{
		if (interface.draw())
		{ 
			if (Execucao::getInstance()->update())
			{
				cout << ">> Simulação finalizada!" << endl;
				interface.setSimulationType(type);
				interface.setTasks(pega_tarefas());
				interface.simulationFinished();
			}
		}
	}

	return 0;
}