#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define QUANTUM 2
#define OCUPADO 1
#define LIVRE 0

//TAREFA
typedef struct Tarefa {
    int id;
    int tempo_execucao;
} Tarefa;
//PROCESSADOR
typedef struct CPU {
    int size;
    int tempo_desligado;
    int ocupado;
}CPU;

//FILA
typedef struct No {
    Tarefa dado;
    struct No *prox;
} No;
typedef struct {
    No *inicio;
    No *fim;
} Fila;
void execucao_passo_a_passo();
void execucao_completa();
void inserir(Fila *f, Tarefa t);
Tarefa remover(Fila *f);
void executar_tarefa(Tarefa *t);
int vazia(Fila *f);
void inicializar_fila(Fila *f);

int main(void) {
    Fila f_tarefas;
    inicializar_fila(&f_tarefas);
    Tarefa t1 = {1, 6};
    Tarefa t2 = {2, 3};

    inserir(&f_tarefas, t1);
    inserir(&f_tarefas, t2);

    while (!vazia(&f_tarefas)) {
        Tarefa atual = remover(&f_tarefas);

        executar_tarefa(&atual);

        if (atual.tempo_execucao > 0) {
            inserir(&f_tarefas, atual);
        }
    }

    return 0;
}


//FUNÇÕES FILA
void inicializar_fila(Fila *f) {
    f->inicio = NULL;
    f->fim = NULL;
}
int vazia(Fila *f) {
    return f->inicio == NULL;
}
void inserir(Fila *f, Tarefa t) {
    No *novo = (No *)malloc(sizeof(No));
    if (novo == NULL) return;
    novo->dado = t;
    novo->prox = NULL;
    if (vazia(f)) {
        f->inicio = novo;
        f->fim = novo;
    } else {
        f->fim->prox = novo;
        f->fim = novo;
    }
}
Tarefa remover(Fila *f) {
    No *temp = f->inicio;
    Tarefa t = temp->dado;
    f->inicio = f->inicio->prox;
    if (f->inicio == NULL) {
        f->fim = NULL;
    }
    free(temp);
    return t;
}

//ROUND-ROBIN - NÃO É PARA USAR -
void executar_tarefa(Tarefa *t) {
    int tempo_gasto = 0;

    while (tempo_gasto < QUANTUM && t->tempo_execucao > 0) {
        clock_t goal = clock() + CLOCKS_PER_SEC;
        while (clock() < goal);

        t->tempo_execucao--;
        tempo_gasto++;
    }
}
