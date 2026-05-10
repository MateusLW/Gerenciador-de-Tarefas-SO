#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

#include "execucao.h"
class CPU;
class Tarefa;
class execucao;
using namespace std;

int quantum;
int num_cpus;
string type;
void readFile();
void inicializa_cpu(vector<CPU*>& f_cpu);

int main() {
    readFile();
    cout << "==> Configuração lida: Tipo=" << type << " | Quantum=" << quantum << endl;
    vector<CPU*> fila_cpu;
    inicializa_cpu(fila_cpu);
    execucao *exe = new execucao(type, fila_cpu, quantum);
    exe->execute();
    return 0;
}

void inicializa_cpu(vector<CPU*>& f_cpu) {
    for (int i=0; i<num_cpus; i++) {
        f_cpu.push_back(new CPU(i));
    }
    cout << "==> Sistema inicializado com " << num_cpus << " CPUs." << endl;
}

void readFile() {

    //Abre arquivo
    ifstream arquivo(R"(C:\Users\mateu\CLionProjects\untitled\tarefas.txt)");

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
            getline(ss, type, ';');
                //Transforma o texto para caixa alta
            transform(type.begin(), type.end(), type.begin(), [](unsigned char c) {
                return toupper(c);
            });
            getline(ss, temp, ';'); quantum = stoi(temp);
            getline(ss, temp, ';'); num_cpus = stoi(temp);
        }
        arquivo.close();
    }
}