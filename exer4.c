#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TAMANHO_FILA 5
#define TOTAL_DE_ITENS 15
#define SINAL_FIM -1


typedef struct {
    int itens[TAMANHO_FILA];
    int inicio, fim, contador;
    pthread_mutex_t mutex;
    pthread_cond_t pode_produzir;
    pthread_cond_t pode_consumir;
} Fila;

Fila fila1;
Fila fila2;

void fila_iniciar(Fila *f) {
    f->inicio = f->fim = f->contador = 0;
    pthread_mutex_init(&f->mutex, NULL);
    pthread_cond_init(&f->pode_produzir, NULL);
    pthread_cond_init(&f->pode_consumir, NULL);
}

void fila_adicionar(Fila *f, int item) {
    pthread_mutex_lock(&f->mutex);
    while (f->contador == TAMANHO_FILA) {
       
        pthread_cond_wait(&f->pode_produzir, &f->mutex);
    }
    f->itens[f->fim] = item;
    f->fim = (f->fim + 1) % TAMANHO_FILA;
    f->contador++;
    
    pthread_cond_signal(&f->pode_consumir);
    pthread_mutex_unlock(&f->mutex);
}

int fila_remover(Fila *f) {
    pthread_mutex_lock(&f->mutex);
    while (f->contador == 0) {
       
        pthread_cond_wait(&f->pode_consumir, &f->mutex);
    }
    int item = f->itens[f->inicio];
    f->inicio = (f->inicio + 1) % TAMANHO_FILA;
    f->contador--;
    
    pthread_cond_signal(&f->pode_produzir);
    pthread_mutex_unlock(&f->mutex);
    return item;
}



void* thread_captura(void *arg) {
    for (int i = 0; i < TOTAL_DE_ITENS; i++) {
        printf("[CAPTURA] Gerou: %d\n", i);
        fila_adicionar(&fila1, i);
    }
    printf("[CAPTURA] Enviou SINAL_FIM.\n");
    fila_adicionar(&fila1, SINAL_FIM);
    return NULL;
}

void* thread_processamento(void *arg) {
    while (1) {
        int item = fila_remover(&fila1);
        if (item == SINAL_FIM) {
            printf("[PROCESSAMENTO] Recebeu SINAL_FIM. Repassando.\n");
            fila_adicionar(&fila2, SINAL_FIM);
            break;
        }
        int item_processado = item * 2; 
        printf("  [PROC] Processou %d -> %d\n", item, item_processado);
        fila_adicionar(&fila2, item_processado);
    }
    return NULL;
}

void* thread_gravacao(void *arg) {
    while (1) {
        int item = fila_remover(&fila2);
        if (item == SINAL_FIM) {
            printf("[GRAVACAO] Recebeu SINAL_FIM. Encerrando.\n");
            break;
        }
        printf("    [GRAVACAO] Gravou item final: %d\n", item);
    }
    return NULL;
}



int main() {
    pthread_t t_captura, t_processamento, t_gravacao;

    fila_iniciar(&fila1);
    fila_iniciar(&fila2);

    printf("Iniciando pipeline...\n");

   
    pthread_create(&t_captura, NULL, thread_captura, NULL);
    pthread_create(&t_processamento, NULL, thread_processamento, NULL);
    pthread_create(&t_gravacao, NULL, thread_gravacao, NULL);

    
    pthread_join(t_captura, NULL);
    pthread_join(t_processamento, NULL);
    pthread_join(t_gravacao, NULL);

    printf("Pipeline finalizado.\n");

    return 0;
}