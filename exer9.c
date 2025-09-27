#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define K_THREADS        4   
#define LEGS_POR_VOLTA   4    
#define DURACAO_SEG      10  

static long long agora_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

static pthread_barrier_t barreira;
static volatile int perna = 0;              
static volatile unsigned long long voltas = 0;
static volatile bool parar = false;

static void* corredor(void* arg) {
    int idx = (int)(long)arg;

    unsigned uso = 200 + (idx * 17 % 80); 

    while (!parar) {
        struct timespec req = { .tv_sec = 0, .tv_nsec = (long)uso * 1000L };
        nanosleep(&req, NULL);

        int rc = pthread_barrier_wait(&barreira);

        if (rc == PTHREAD_BARRIER_SERIAL_THREAD) {
            int p = ++perna;
            if (p >= LEGS_POR_VOLTA) {
                perna = 0;
                unsigned long long v = ++voltas;
                printf("Volta concluida #%llu\n", v);
                fflush(stdout);
            }
        }
    }
    return NULL;
}

int main(void) {
    printf("Iniciando corrida: K=%d, pernas/volta=%d, duracao=%ds\n",
           K_THREADS, LEGS_POR_VOLTA, DURACAO_SEG);

    if (pthread_barrier_init(&barreira, NULL, K_THREADS) != 0) {
        perror("pthread_barrier_init");
        return 1;
    }

    pthread_t th[K_THREADS];
    for (int i = 0; i < K_THREADS; ++i) {
        if (pthread_create(&th[i], NULL, corredor, (void*)(long)i) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    long long t0 = agora_ns();
    while ((agora_ns() - t0) < (long long)DURACAO_SEG * 1000000000LL) {
        struct timespec req = { .tv_sec = 0, .tv_nsec = 50 * 1000 * 1000 }; 
        nanosleep(&req, NULL);
    }
    parar = true;

    for (int i = 0; i < K_THREADS; ++i) pthread_join(th[i], NULL);
    pthread_barrier_destroy(&barreira);

    double voltas_por_min = (double)voltas * (60.0 / DURACAO_SEG);
    printf("\nVoltas totais: %llu\nTaxa: %.2f voltas/min\n",
           voltas, voltas_por_min);
    return 0;
}