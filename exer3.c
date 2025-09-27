#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>

#define M 10
#define T 4
#define TRANSFERS 100000

//para compila use :  gcc -o transf exer3.c
// ./transf
//usei maquina virtual linux wsl debian

int contas[M];
pthread_mutex_t locks[M];

// soma total
int soma_total() {
    int soma = 0;
    for (int i = 0; i < M; i++) soma += contas[i];
    return soma;
}

// trava duas contas
void lock_two(int a, int b) {
    if (a == b) {
        pthread_mutex_lock(&locks[a]);
        return;
    }
    if (a < b) {
        pthread_mutex_lock(&locks[a]);
        pthread_mutex_lock(&locks[b]);
    } else {
        pthread_mutex_lock(&locks[b]);
        pthread_mutex_lock(&locks[a]);
    }
}

// destrava duas contas
void unlock_two(int a, int b) {
    if (a == b) {
        pthread_mutex_unlock(&locks[a]);
        return;
    }
    if (a < b) {
        pthread_mutex_unlock(&locks[b]);
        pthread_mutex_unlock(&locks[a]);
    } else {
        pthread_mutex_unlock(&locks[a]);
        pthread_mutex_unlock(&locks[b]);
    }
}

//sem trava 
void* transferencias_sem_trava(void* arg) {
    int id = *(int*)arg;
    unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)id;
    for (int i = 0; i < TRANSFERS; i++) {
        int origem  = rand_r(&seed) % M;
        int destino = rand_r(&seed) % M;
        while (destino == origem) destino = rand_r(&seed) % M;
        int valor   = (rand_r(&seed) % 100) + 1;

        if (contas[origem] >= valor) {
            contas[origem] -= valor;
            contas[destino] += valor;
        }
    }
    return NULL;
}

//com trava
void* transferencias_com_trava(void* arg) {
    int id = *(int*)arg;
    unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)id ^ 0x9e3779b9;
    for (int i = 0; i < TRANSFERS; i++) {
        int origem  = rand_r(&seed) % M;
        int destino = rand_r(&seed) % M;
        while (destino == origem) destino = rand_r(&seed) % M;
        int valor   = (rand_r(&seed) % 100) + 1;

        lock_two(origem, destino);

        if (contas[origem] >= valor) {
            contas[origem] -= valor;
            contas[destino] += valor;
        }

        unlock_two(origem, destino);
    }
    return NULL;
}

int main() {
    for (int i = 0; i < M; i++) {
        contas[i] = 1000;
        pthread_mutex_init(&locks[i], NULL);
    }

    int soma_inicial = soma_total();
    pthread_t threads[T];
    int ids[T];

    //SEM trava
    for (int i = 0; i < T; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, transferencias_sem_trava, &ids[i]);
    }
    for (int i = 0; i < T; i++) pthread_join(threads[i], NULL);

    int soma_final_sem = soma_total();
    printf("Sem trava: soma inicial = %d, soma final = %d\n",
           soma_inicial, soma_final_sem);

    // reset das contas para testar a versão correta
    for (int i = 0; i < M; i++) contas[i] = 1000;

    // COM trava
    for (int i = 0; i < T; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, transferencias_com_trava, &ids[i]);
    }
    for (int i = 0; i < T; i++) pthread_join(threads[i], NULL);

    int soma_final_com = soma_total();
    printf("Com trava: soma inicial = %d, soma final = %d\n",
           soma_inicial, soma_final_com);

    
    assert(soma_inicial == soma_final_com);

    
    for (int i = 0; i < M; i++) pthread_mutex_destroy(&locks[i]);

    return 0;
}
