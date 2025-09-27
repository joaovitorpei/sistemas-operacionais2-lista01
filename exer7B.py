import threading as th
import time
from random import random

# ======== configurar aqui ========
N = 5                       # quantidade de filósofos/garfos
DURACAO_SEGUNDOS = 10       # tempo total da simulação
RELATAR_EVENTOS = False     # True = imprime quando começa/termina de comer
# =================================

garfos = [th.Lock() for _ in range(N)]
portao = th.Semaphore(N - 1)   # permite no máx. 4 tentando de cada vez
parar = False

# métricas
refeicoes = [0] * N
maior_espera = [0.0] * N  # em segundos

def dormir(seg): time.sleep(seg)
def pensar(): dormir(0.01 + 0.02 * random())
def comer():  dormir(0.01 + 0.02 * random())

# Mitigação simples de fome:
def mitigar_fome(i: int):
    while not parar and refeicoes[i] > min(refeicoes) + 1:
        dormir(0.01)

def filosofo(i: int):
    esq = i
    dir = (i + 1) % N
    while not parar:
        pensar()
        mitigar_fome(i)
        t0 = time.perf_counter()

        # Porta global: limita concorrência (evita deadlock)
        if not portao.acquire(timeout=0.1):
            continue
        try:
            # Ordem local funciona bem com o semáforo ativo
            with garfos[esq]:
                with garfos[dir]:
                    if RELATAR_EVENTOS: print(f"Filósofo {i} começou a comer.")
                    esperou = time.perf_counter() - t0
                    maior_espera[i] = max(maior_espera[i], esperou)
                    refeicoes[i] += 1
                    comer()
                    if RELATAR_EVENTOS: print(f"Filósofo {i} terminou de comer.")
        finally:
            portao.release()

def main():
    ts = [th.Thread(target=filosofo, args=(i,), daemon=True) for i in range(N)]
    for t in ts: t.start()

    dormir(DURACAO_SEGUNDOS)
    global parar; parar = True
    for t in ts: t.join(timeout=0.2)

    print("\n===== RESUMO FINAL (SEMAFORO) =====")
    print(f"Tempo: {DURACAO_SEGUNDOS}s\n")
    for i in range(N):
        print(f"Filósofo {i}: refeições={refeicoes[i]:4d}  maior_espera={maior_espera[i]*1000:5.1f} ms")

if __name__ == "__main__":
    main()
