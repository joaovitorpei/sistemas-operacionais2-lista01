#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NUM_CAVALOS 6
#define DISTANCIA_MAX 100

const char* nomes_cavalos[NUM_CAVALOS] = {
    "Relâmpago", "Pé de Pano", "Trovão", "Estrela Cadente", "Furacão", "Cometa"
};

typedef struct {
    int id;
    char nome[50];
    int velocidade_base;
    int distancia_percorrida;
    int colocacao;
    unsigned int seed;
} Cavalo;

Cavalo cavalos[NUM_CAVALOS];
int colocacao_atual = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int vencedor_aposta = 0;
int cavalo_escolhido = 0;

void* correr(void* arg) {
    Cavalo* cavalo = (Cavalo*)arg;
    
    printf("🐎 %s começou a correr!\n", cavalo->nome);
    
    while (cavalo->distancia_percorrida < DISTANCIA_MAX) {
        int avanco = cavalo->velocidade_base + (rand_r(&cavalo->seed) % 8);
        cavalo->distancia_percorrida += avanco;
        
        if (cavalo->distancia_percorrida > DISTANCIA_MAX) {
            cavalo->distancia_percorrida = DISTANCIA_MAX;
        }
        
        // Ajuste no tempo de espera: tempo_espera = (180ms a 50ms) - (velocidade * 15ms) + (rand 0-50ms)
        int tempo_espera = 180000 - (cavalo->velocidade_base * 15000);
        tempo_espera += (rand_r(&cavalo->seed) % 50000);
        usleep(tempo_espera);
        
        printf("🐎 %s: %d metros\n", cavalo->nome, cavalo->distancia_percorrida);
    }
    
    pthread_mutex_lock(&mutex);
    cavalo->colocacao = colocacao_atual++;
    
    if (cavalo->colocacao == 1) {
        vencedor_aposta = cavalo->id;
    }
    
    printf("🎉 %s terminou em %dº lugar!\n", cavalo->nome, cavalo->colocacao);
    pthread_mutex_unlock(&mutex);
    
    return NULL;
}

// FUNÇÃO CORRIGIDA: Adicionada a velocidade base na saída para corresponder aos 3 argumentos.
void mostrar_cavalos() {
    printf("\n🐎 CAVALOS DISPONÍVEIS PARA APOSTA:\n");
    printf("==================================\n");
    for (int i = 0; i < NUM_CAVALOS; i++) {
        printf("%d - %s (Vel: %d)\n", 
                cavalos[i].id, cavalos[i].nome, cavalos[i].velocidade_base);
    }
    printf("==================================\n");
}

int fazer_aposta() {
    int aposta;
    
    printf("\n💵 FAÇA SUA APOSTA!\n");
    printf("Em qual cavalo você aposta? (1 a %d): ", NUM_CAVALOS);
    
    while (1) {
        if (scanf("%d", &aposta) != 1) {
            printf("Entrada inválida! Digite um número entre 1 e %d: ", NUM_CAVALOS);
            while (getchar() != '\n');
            continue;
        }
        
        if (aposta >= 1 && aposta <= NUM_CAVALOS) {
            break;
        }
        printf("Cavalo inválido! Digite um número entre 1 e %d: ", NUM_CAVALOS);
    }
    
    return aposta;
}

void mostrar_resultado_aposta() {
    printf("\n🎯 RESULTADO DA APOSTA:\n");
    printf("=======================\n");
    
    const char* nome_cavalo_escolhido = cavalos[cavalo_escolhido - 1].nome;
    const char* nome_vencedor = cavalos[vencedor_aposta - 1].nome;
    
    if (vencedor_aposta == cavalo_escolhido) {
        printf("🏆 PARABÉNS! Você acertou!\n");
        printf("💰 %s VENCEU a corrida!\n", nome_cavalo_escolhido);
        printf("🎯 Você é um ótimo apostador!\n");
    } else {
        printf("❌ Que pena! Você errou.\n");
        printf("😢 %s não venceu a corrida.\n", nome_cavalo_escolhido);
        printf("⭐ O vencedor foi %s\n", nome_vencedor);
        printf("💸 Melhor sorte na próxima vez!\n");
    }
    printf("=======================\n");
}

void animacao_inicio() {
    printf("\n🏇 PREPARANDO A CORRIDA DE CAVALOS!\n");
    printf("🔔 Sininho tocando...\n");
    sleep(1);
    printf("🚩 Cavalos se posicionando...\n");
    sleep(1);
    printf("🎯 Apostas fechadas!\n");
    sleep(1);
    printf("🏁 E... LÁ VÃO ELES!\n\n");
    sleep(1);
}

int main() {
    pthread_t threads[NUM_CAVALOS];
    srand(time(NULL));
    
    for (int i = 0; i < NUM_CAVALOS; i++) {
        cavalos[i].id = i + 1;
        strcpy(cavalos[i].nome, nomes_cavalos[i]);
        cavalos[i].velocidade_base = (rand() % 6) + 4; // 4-9
        cavalos[i].distancia_percorrida = 0;
        cavalos[i].colocacao = 0;
        cavalos[i].seed = time(NULL) + cavalos[i].id;
    }
    
    printf("🎰 BEM-VINDO AO HIPISMO DIGITAL! 🎰\n");
    mostrar_cavalos();
    cavalo_escolhido = fazer_aposta();
    
    printf("\n✅ Você apostou no %s! Boa sorte!\n", cavalos[cavalo_escolhido - 1].nome);
    
    animacao_inicio();
    
    // Cria as threads
    for (int i = 0; i < NUM_CAVALOS; i++) {
        pthread_create(&threads[i], NULL, correr, &cavalos[i]);
    }
    
    for (int i = 0; i < NUM_CAVALOS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\n🏆 CLASSIFICAÇÃO FINAL:\n");
    printf("=======================\n");
    for (int i = 1; i <= NUM_CAVALOS; i++) {
        for (int j = 0; j < NUM_CAVALOS; j++) {
            if (cavalos[j].colocacao == i) {
                printf("%dº lugar: %s (vel: %d)\n", 
                        i, cavalos[j].nome, cavalos[j].velocidade_base);
                break;
            }
        }
    }
    
    mostrar_resultado_aposta();
    
    pthread_mutex_destroy(&mutex);
    return 0;
}