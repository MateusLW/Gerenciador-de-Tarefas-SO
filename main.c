#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define OCUPADO 1
#define LIVRE 0
int quantum;
char escal[5];

/* -PARA ADICIONAR:                                                                               *
 *                                                                                                *
 *  Fazer o metodo de escalonamento PRIOP                                                         *
 *  Resolver questão da localização do arquivo                                                    *
 *  Interface de Visuzalização com sistema de cores por tarefa                                    *
 *                                                                                                */

typedef struct Tarefa {
    int id;
    char* cor;
    int tempo_ingresso;
    int tempo_execucao;
    int prioridade;

} Tarefa;

typedef struct CPU {
    int id;
    Tarefa* t;
    int ocupado;
    int tempo_desligado;
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
void ingresso_tarefa(int relogio, Fila_Tarefas *f);
void SRTF(Fila_CPU *c, Fila_Tarefas *t);
void execucao(Fila_CPU *f);

void escalonamento_config(Fila_CPU* c);

        /*  FUNÇÕES DE FILAS    */
void inserir(Fila_Tarefas *f, Tarefa* t);
void inicializar_fila(Fila_Tarefas *f);
void inicializar_fila_CPU(Fila_CPU *f, int size);

int main() {
    Fila_Tarefas* f_tarefas = malloc(sizeof(Fila_Tarefas));
    inicializar_fila(f_tarefas);
    Fila_CPU* f_cpu = malloc(sizeof(Fila_CPU));

    // Traz configuracao do escalonamento
    escalonamento_config(f_cpu);
    printf("Iniciando Escalonamento SRTF...\n");

    /*  ESCALONA ENQUANTO A FILA DE ESPERA NÃO ESTIVER VAZIA    */
    int processando = 1;
    int relogio=0;
    while (processando) {
        ingresso_tarefa(relogio, f_tarefas);    /*  -Entrada das tarefas na fila-  */
        SRTF(f_cpu, f_tarefas);                 /*         -Escalonamento-         */
        for (int i=0; i < quantum; i++) {
            /*            -Execução-           */
            execucao(f_cpu);
            relogio++;
        }

        //Checa se fila está vazia e se ainda tem algo sendo processado pela CPU
        processando = (f_tarefas->inicio != NULL);
        for(No_CPU* aux = f_cpu->inicio; aux != NULL; aux = aux->prox) {
            if(aux->dado->t != NULL) processando = 1;
        }
    }

    printf("Todas as tarefas concluidas em %d segundos. \n", relogio-1);
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
            printf("[CPU %d] Trocou para Tarefa %d (Restante: %d)\n",cpu->id, cpu->t->id, cpu->t->tempo_execucao);
        }
    }
}

void execucao(Fila_CPU *f) {
    // Simula a passagem de 1 unidade de tempo
    for (No_CPU* aux = f->inicio; aux != NULL; aux = aux->prox) {
        CPU* cpu = aux->dado;
        if (cpu->t != NULL) {
            cpu->t->tempo_execucao--;
            printf("Executando ID %d em CPU %d | Restante: %d\n", cpu->t->id,cpu->id, cpu->t->tempo_execucao);

            if (cpu->t->tempo_execucao <= 0) {
                printf("Tarefa %d FINALIZADA.\n", cpu->t->id);
                cpu->t = NULL;
                cpu->ocupado = LIVRE;
            }
        }
        else
            cpu->tempo_desligado++;
    }
    printf("--------------------------\n");
}
void ingresso_tarefa(int relogio, Fila_Tarefas *f) {
    FILE* arquivo;
    //Abre arquivo
    arquivo = fopen("C:\\Users\\mateu\\CLionProjects\\Projeto1-SO\\tarefas.txt", "r");
    int id, tempo_execucao, tempo_ingresso, prioridade;
    char cor[5];
    if (arquivo == NULL)
        printf("Erro ao abrir o arquivo");
    else {
        //elimina a primera linha
        char descarta[256];
        fgets(descarta, sizeof(descarta), arquivo);

        //Percorre cada linha pegando seus valores
        while (fscanf(arquivo, "%d;%[^;];%d;%d;%d", &id, cor, &tempo_ingresso, &tempo_execucao, &prioridade )!=EOF) {
            if (tempo_ingresso == relogio) {
                /*  -Adiciona a tarefa na fila-  */
                Tarefa* nova_t = (Tarefa*) malloc(sizeof(Tarefa));
                nova_t->id = id;
                nova_t->tempo_ingresso = tempo_ingresso;
                nova_t->tempo_execucao = tempo_execucao;
                nova_t->prioridade = prioridade;

                inserir(f, nova_t);
            }
        }
    }
}

void escalonamento_config(Fila_CPU* c) {
    FILE* arquivo;
    arquivo = fopen("C:\\Users\\mateu\\CLionProjects\\Projeto1-SO\\tarefas.txt", "r");
    int num_cpus;
    if (arquivo == NULL)
        printf("Erro ao abrir o arquivo");
    else {
        fscanf(arquivo, "%[^;];%d;%d", escal, &quantum, &num_cpus);
        inicializar_fila_CPU(c, num_cpus);
    }

}

void inserir(Fila_Tarefas *f, Tarefa* t) {
        /*  -Cria tarefa-   */
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
            /*  -Cria CPU's-   */
        No_CPU* novo = malloc(sizeof(No_CPU));
        novo->dado = malloc(sizeof(CPU));
        novo->dado->id = i+1;
        novo->dado->t = NULL;
        novo->dado->ocupado = LIVRE;
        novo->dado->tempo_desligado = 0;
        novo->prox = NULL;
        if (f->inicio == NULL) f->inicio = novo;
        else f->fim->prox = novo;
        f->fim = novo;
    }
}