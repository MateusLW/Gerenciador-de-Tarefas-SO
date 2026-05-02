#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define OCUPADO 1
#define LIVRE 0

/* -PARA ADICIONAR:                                     *
 *                                                      *
 *  Parâmetro de tempo de entrada da tarefa, junto com  *
 *  seu sistema para adicionar a tarefa na fila somente *
 *  chegar o tempo de sua entrada.                      *
 *                                                      *
 *  Relógio Global.                                     *
 *                                                      */

typedef struct Tarefa {
    int id;
    int tempo_execucao;
} Tarefa;

typedef struct CPU {
    Tarefa* t;
    int ocupado;
} CPU;

typedef struct No_Tarefa {
    Tarefa* dado;
    struct No_Tarefa *ant;
    struct No_Tarefa *prox;
} No_Tarefa;

typedef struct Fila_Tarefas {
    No_Tarefa *inicio;
    No_Tarefa *fim;
} Fila_Tarefas;

typedef struct No_CPU {
    CPU* dado;
    struct No_CPU *prox;
} No_CPU;

typedef struct Fila_CPU {
    No_CPU *inicio;
    No_CPU *fim;
} Fila_CPU;

        /* FUNÇÕES DE EXECUÇÃO  */
void SRTF(Fila_CPU *c, Fila_Tarefas *t);
void execucao(Fila_CPU *f);

        /*  FUNÇÕES DE FILAS    */
void inserir(Fila_Tarefas *f, Tarefa* t);
void inicializar_fila(Fila_Tarefas *f);
void inicializar_fila_CPU(Fila_CPU *f, int size);

int main() {
    Fila_Tarefas* f_tarefas = malloc(sizeof(Fila_Tarefas));
    inicializar_fila(f_tarefas);

    // Tarefas teste
    Tarefa t1 = {1, 6};
    Tarefa t2 = {2, 3};
    Tarefa t3 = {3, 1};

    Fila_CPU* f_cpu = malloc(sizeof(Fila_CPU));

    // CPU teste
    inicializar_fila_CPU(f_cpu, 1);

    //Tarefas em fila de espera
    inserir(f_tarefas, &t1);
    inserir(f_tarefas, &t2);
    inserir(f_tarefas, &t3);

    printf("Iniciando Escalonamento SRTF...\n");

    /*  ESCALONA ENQUANTO A FILA DE ESPERA NÃO ESTIVER VAZIA    */
    int processando = 1;
    while (processando) {

        SRTF(f_cpu, f_tarefas); /*  -Escalonamento-  */
        execucao(f_cpu);        /*    -Execução-     */

        //Checa se fila está vazia e se ainda tem algo sendo processado pela CPU
        processando = (f_tarefas->inicio != NULL);
        for(No_CPU* aux = f_cpu->inicio; aux != NULL; aux = aux->prox) {
            if(aux->dado->t != NULL) processando = 1;
        }
    }

    printf("Todas as tarefas concluidas.\n");
    return 0;
}

    /*      -FUNÇÕES DE ESCALONAMENTO-       */
void SRTF(Fila_CPU *c, Fila_Tarefas *t) {
    for (No_CPU* cpu_node = c->inicio; cpu_node != NULL; cpu_node = cpu_node->prox) {
        CPU* cpu = cpu_node->dado;

        /*  Procura menor tempo de execução de uma tarefa   */
        No_Tarefa* menor_no = NULL;
            //Caso CPU esteja LIVRE
        int menor_tempo = (cpu->t != NULL) ? cpu->t->tempo_execucao : INT_MAX;

        for (No_Tarefa* curr = t->inicio; curr != NULL; curr = curr->prox) {
            if (curr->dado->tempo_execucao < menor_tempo) {
                menor_tempo = curr->dado->tempo_execucao;
                menor_no = curr;
            }
        }

        /*  Realiza a troca */
        if (menor_no != NULL) {
            Tarefa* tarefa_saindo = cpu->t;
            cpu->t = menor_no->dado;
            cpu->ocupado = OCUPADO;

            // Se a tarefa for a primera da fila
            if (menor_no->ant) menor_no->ant->prox = menor_no->prox;
            else t->inicio = menor_no->prox;

            // Se a tarefa for a última da fila
            if (menor_no->prox) menor_no->prox->ant = menor_no->ant;
            else t->fim = menor_no->ant;

            free(menor_no);

            // Se havia uma tarefa na CPU, ela volta para a fila
            if (tarefa_saindo != NULL) {
                inserir(t, tarefa_saindo);
            }
            printf("[CPU] Trocou para Tarefa %d (Restante: %d)\n", cpu->t->id, cpu->t->tempo_execucao);
        }
    }
}

void execucao(Fila_CPU *f) {
    // Simula a passagem de 1 unidade de tempo
    for (No_CPU* aux = f->inicio; aux != NULL; aux = aux->prox) {
        CPU* cpu = aux->dado;
        if (cpu->t != NULL) {
            cpu->t->tempo_execucao--;
            printf("Executando ID %d | Restante: %d\n", cpu->t->id, cpu->t->tempo_execucao);

            if (cpu->t->tempo_execucao <= 0) {
                printf("Tarefa %d FINALIZADA.\n", cpu->t->id);
                cpu->t = NULL;
                cpu->ocupado = LIVRE;
            }
        }
    }
    printf("--------------------------\n");
}

void inserir(Fila_Tarefas *f, Tarefa* t) {
    No_Tarefa *novo = malloc(sizeof(No_Tarefa));
    novo->dado = t;
    novo->prox = NULL;
    novo->ant = f->fim;
    if (f->inicio == NULL) f->inicio = novo;
    else f->fim->prox = novo;
    f->fim = novo;
}

void inicializar_fila(Fila_Tarefas *f) {
    f->inicio = f->fim = NULL;
}

void inicializar_fila_CPU(Fila_CPU* f, int size) {
    f->inicio = f->fim = NULL;
    for (int i = 0; i < size; i++) {
        No_CPU* novo = malloc(sizeof(No_CPU));
        novo->dado = malloc(sizeof(CPU));
        novo->dado->t = NULL;
        novo->dado->ocupado = LIVRE;
        novo->prox = NULL;
        if (f->inicio == NULL) f->inicio = novo;
        else f->fim->prox = novo;
        f->fim = novo;
    }
}