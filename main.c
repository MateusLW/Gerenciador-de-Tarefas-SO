#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define quantum 2
struct Tarefa { //TAREFAS
    int tempo_execucao;
};
struct No {                 // INICIO FUNÇÕES FILA
    struct Tarefa *dado;
    struct No *proximo;
};
struct Fila {
    struct No *frente;
    struct No *tras;
};
void enfileirar(struct Fila *f, struct Tarefa *nova_tarefa) {
    struct No *novo_no = malloc(sizeof(struct No));
    novo_no->dado = nova_tarefa;
    novo_no->proximo = NULL;

    if (f->tras == NULL) {
        f->frente = f->tras = novo_no;
        return;
    }
    f->tras->proximo = novo_no;
    f->tras = novo_no;
}
struct Tarefa desenfileirar(struct Fila *f) {
    if (f->frente == NULL) {
        printf("Fila vazia!\n");
        exit(1);
    }
    struct No *temp = f->frente;
    struct Tarefa *t = temp->dado;
    f->frente = f->frente->proximo;

    if (f->frente == NULL) f->tras = NULL;
    free(temp);
    return *t;
}
int empty(struct Fila *f) {
    if (f->frente == NULL)
        return 1;
    return 0;                           //FIM DAS FUNÇÕES DA FILA
}
struct Tarefa inicializar_tarefa() {    //INICIALIZA TAREFA
    srand(time(NULL));
    struct Tarefa tarefa;
    tarefa.tempo_execucao = rand()%10 + 1; //ATRIBUI VALOR ALEATÓRIO PARA O TEMPO DE EXECUÇÃO
    return tarefa;
}
void executar_tarefa (struct Tarefa *tarefa) {      //SIMULA EXECUÇÃO NO PROCESSADOR
    time_t inicio, agora;
    int tempo_execucao = 0;
    while (tempo_execucao < quantum && tarefa->tempo_execucao > 0) { //ENQUANTO EXECUTA NO TEMPO DO QUANTUM
        time(&inicio);                                              //E ENQUANTO A TAREFA AINDA PODE EXECUTAR
        //REGISTA HORARIO DE INICIO
        do {
            time(&agora); //ATUALIZA O HORARIO ATUAL ATE TER A DIFERENÇA DE 1 SEGUNDO
        }while(difftime(agora, inicio) < 1.0);
        tempo_execucao += 1; //REGISTRA TEMPO DE EXECUÇÃO
        tarefa->tempo_execucao -= 1; //REGISTRA EXECUÇÃO DA TAREFA
    }
}
int main(void) {
    struct Tarefa tarefaA = inicializar_tarefa(); //TAREFAS
    struct Tarefa tarefaB = inicializar_tarefa();
    struct Tarefa tarefaC = inicializar_tarefa();
    struct Fila f;
    enfileirar(&f, &tarefaA); //FILA
    enfileirar(&f, &tarefaB);
    enfileirar(&f, &tarefaC);
    while (!empty(&f)) { //ENQUANTO A FILA NÃO ESTA VAZIA
        executar_tarefa(&tarefaA);
    }

    return 0;
}
