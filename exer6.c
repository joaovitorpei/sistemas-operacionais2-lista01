#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define N_THREADS 4             
#define NUM_ELEMENTOS 10000000  
#define MAX_VALOR 10000

int vetor[NUM_ELEMENTOS];
long long soma_total = 0;
int histograma_global[MAX_VALOR];

pthread_mutex_t mutex_soma = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_hist = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int inicio;
    int fim;
} Bloco;

void* map_reduce(void* arg) {
    Bloco* bloco = (Bloco*)arg;
    long long soma_local = 0;
    int hist_local[MAX_VALOR] = {0};

    for (int i = bloco->inicio; i < bloco->fim; i++) {
        int val = vetor[i];
        soma_local += val;
        hist_local[val]++;
    }

    pthread_mutex_lock(&mutex_soma);
    soma_total += soma_local;
    pthread_mutex_unlock(&mutex_soma);

    pthread_mutex_lock(&mutex_hist);
    for (int i = 0; i < MAX_VALOR; i++)
        histograma_global[i] += hist_local[i];
    pthread_mutex_unlock(&mutex_hist);

    return NULL;
}

double tempo_agora() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

int main() {
    srand(time(NULL));
    for (int i = 0; i < NUM_ELEMENTOS; i++)
        vetor[i] = rand() % MAX_VALOR;

    pthread_t threads[N_THREADS];
    Bloco blocos[N_THREADS];

    int tam_bloco = NUM_ELEMENTOS / N_THREADS;

    double inicio = tempo_agora();

    for (int i = 0; i < N_THREADS; i++) {
        blocos[i].inicio = i * tam_bloco;
        blocos[i].fim = (i == N_THREADS - 1) ? NUM_ELEMENTOS : (i + 1) * tam_bloco;
        pthread_create(&threads[i], NULL, map_reduce, &blocos[i]);
    }

    for (int i = 0; i < N_THREADS; i++)
        pthread_join(threads[i], NULL);

    double fim = tempo_agora();

    printf("Threads: %d\n", N_THREADS);
    printf("Tempo: %.4f segundos\n", fim - inicio);
    printf("Soma total: %lld\n", soma_total);

    printf("Top 10 valores mais frequentes:\n");
    int mostrados = 0;
    for (int i = 0; i < MAX_VALOR && mostrados < 10; i++) {
        if (histograma_global[i] > 0) {
            printf("Valor %d: %d vezes\n", i, histograma_global[i]);
            mostrados++;
        }
    }

    return 0;
}
