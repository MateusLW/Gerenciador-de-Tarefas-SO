#include "execucao.h"

execucao::execucao(string type, vector<CPU*> cpu_list, int quantum) {
    this->type = std::move(type); //SRTF OU PRIOP
    this->cpu_list = std::move(cpu_list);
    this->quantum = quantum;
    this->relogio = 0;
}

void execucao::entra_tarefa() {

    //Abre arquivo
    ifstream arquivo(R"(C:\Users\mateu\CLionProjects\untitled\tarefas.txt)");
    if (!arquivo.is_open()) return;


    string linha;
    //Pula a primeira linha
    getline(arquivo, linha);

    //Percorre todas as linhas
    while (getline(arquivo, linha)) {
        stringstream ss(linha);

        //Armazena itens separados por delimitador
        string temp, cor;
        int id, t_ingresso, t_execucao, prioridade;
        getline(ss, temp, ';'); id = stoi(temp);
        getline(ss, cor, ';');
        getline(ss, temp, ';'); t_ingresso = stoi(temp);
        getline(ss, temp, ';'); t_execucao = stoi(temp);
        getline(ss, temp, ';'); prioridade = stoi(temp);

        //Se tarefa já está em seu tempo de ingresso na fila
        if (t_ingresso == relogio) {
            Tarefa* nova_t = new Tarefa(id, cor, t_ingresso, t_execucao, prioridade);
            tarefas.push_back(nova_t);
            cout << "  [FILA] Tarefa ID " << id << " chegou ao sistema." << endl;
        }
    }
}

void execucao::execute() {
    cout << "\n*** Iniciando Simulacao " << type << " ***\n" << endl;
    if (type == "SRTF") {
        bool processando = true;
        entra_tarefa();
        //Enquanto a fila e os processadores não estão vazios
        while (processando) {
            cout << "\n------------------------------------------" << endl;

            SRTF();
            quantum_tempo();

            // Verifica se ainda há trabalho a fazer
            processando = !tarefas.empty();
            for (CPU* cpu : cpu_list) {
                if (cpu->getTarefa() != nullptr) processando = true;
            }

        }
        cout << "\n*** Simulacao Finalizada no tempo " << relogio << "s ***" << endl;
    }
    else if (type == "PRIOP") {
        bool processando = true;
        entra_tarefa();
        //Enquanto a fila e os processadores não estão vazios
        while (processando) {
            cout << "\n------------------------------------------" << endl;
            cout << "[RELOGIO: " << relogio << "s]" << endl;

            PRIOP();
            quantum_tempo();

            // Verifica se ainda há trabalho a fazer
            processando = !tarefas.empty();
            for (CPU* cpu : cpu_list) {
                if (cpu->getTarefa() != nullptr) processando = true;
            }

        }
        cout << "\n*** Simulacao Finalizada no tempo " << relogio << "s ***" << endl;
    }
}

void execucao::SRTF() {
    for (int n = 0; n < cpu_list.size(); n++) {
        if (tarefas.empty()) break;

        //Procura tarefa com menor tempo de execução restante
        int idx_fila_menor = 0;
        for (int i = 1; i < tarefas.size(); i++) {
            if (tarefas[i]->getTempoRestante() < tarefas[idx_fila_menor]->getTempoRestante()) {
                idx_fila_menor = i;
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
            cout << "  [ALOCAR] CPU " << cpu_alvo->getId() << " assumiu Tarefa " << melhor_da_fila->getId() << endl;
            cpu_alvo->setTarefa(melhor_da_fila);
            tarefas.erase(tarefas.begin() + idx_fila_menor);
        }

        //Processador com tarefa de maior tempo restante comparado ao de menor tempo da fila de prontos
        else if (cpu_alvo != nullptr && melhor_da_fila->getTempoRestante() < cpu_alvo->getTarefa()->getTempoRestante()) {
            Tarefa* saindo = cpu_alvo->getTarefa();

            cout << "  [PREEMPCAO] CPU " << cpu_alvo->getId() << ": Substituindo ID " << saindo->getId()
                 << " (" << saindo->getTempoRestante() << "s) por ID " << melhor_da_fila->getId()
                 << " (" << melhor_da_fila->getTempoRestante() << "s)" << endl;

            cpu_alvo->setTarefa(melhor_da_fila);
            tarefas.erase(tarefas.begin() + idx_fila_menor);
            tarefas.push_back(saindo);
        }
        //Nenhuma tarefa na fila de prontos é menor que qualquer tarefa nos processadores
        else {
            break;
        }
    }
}

void execucao::PRIOP() {
    for (int n = 0; n < cpu_list.size(); n++) {
        if (tarefas.empty()) break;

        //Procura tarefa com maior prioridade da fila
        int idx_fila_maior = 0;
        for (int i = 1; i < tarefas.size(); i++) {
            if (tarefas[i]->getPrioridade() > tarefas[idx_fila_maior]->getPrioridade()) {
                idx_fila_maior = i;
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
            cout << "  [ALOCAR] CPU " << cpu_alvo->getId() << " assumiu Tarefa " << melhor_da_fila->getId() << endl;
            cpu_alvo->setTarefa(melhor_da_fila);
            tarefas.erase(tarefas.begin() + idx_fila_maior);
        }

        //Processador com tarefa de menor prioridade comparado ao de maior prioridade da fila de prontos
        else if (cpu_alvo != nullptr && melhor_da_fila->getPrioridade() > cpu_alvo->getTarefa()->getPrioridade()) {
            Tarefa* saindo = cpu_alvo->getTarefa();

            cout << "  [PREEMPCAO] CPU " << cpu_alvo->getId() << ": Substituindo ID " << saindo->getId()
                 << " (Prioridade " << saindo->getPrioridade() << ") por ID " << melhor_da_fila->getId()
                 << " (Prioridade " << melhor_da_fila->getPrioridade() << ")" << endl;

            cpu_alvo->setTarefa(melhor_da_fila);
            tarefas.erase(tarefas.begin() + idx_fila_maior);
            tarefas.push_back(saindo);
        }
        //Nenhuma tarefa na fila de prontos é menor que qualquer tarefa nos processadores
        else {
            break;
        }
    }
}

void execucao::quantum_tempo() {
    for (int i = 0; i < quantum; i++) {
        relogio++;
        cout << "[RELOGIO: " << relogio << "s]" << endl;
        entra_tarefa();
        for (CPU* cpu : cpu_list) {
            Tarefa* t = cpu->getTarefa();

            //Tarefa em processador executa
            if (t != nullptr) {
                t->executarUnidade();
                cout << "  [CPU " << cpu->getId() << "] Executando Tarefa " << t->getId()
                     << " | Restante: " << t->getTempoRestante() << "s" << endl;

                //Libera CPU caso seja concluida
                if (t->isConcluida()) {
                    cout << "  [TERMINOU] Tarefa " << t->getId() << " concluida na CPU " << cpu->getId() << endl;
                    cpu->liberarCPU();
                }
            //Processador sem executar nenhuma tarefa
            } else {
                cpu->unidadeDesligado();
            }
        }
    }
}