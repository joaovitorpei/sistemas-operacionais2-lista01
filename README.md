## QUESTÃO 1
Relatório – Corrida de Cavalos com Threads e Mutex (Simulação com Apostas)  

## Objetivo  

Simular uma corrida de cavalos com múltiplas threads concorrentes, onde cada cavalo é controlado por uma thread independente, percorrendo uma distância até a linha de chegada.  

Além disso, o programa:  

Permite o usuário apostar em um cavalo antes da corrida;  
Apresenta o progresso dos cavalos em tempo real;  
Garante que a ordem de chegada (colocação) seja registrada de forma correta e sem condições de corrida, usando mutex para sincronização.  
Estrutura Geral do Código  

### Inicialização:  

Um vetor com 6 cavalos é criado. Cada cavalo possui:  

id, nome, velocidade_base, distancia_percorrida, colocacao e uma seed para aleatoriedade.  
A velocidade base é gerada aleatoriamente entre 4 e 9 para cada cavalo.  

Threads:  

Cada cavalo é representado por uma thread, que executa a função "correr".  
 
### Dentro dessa função:  

O cavalo avança somando sua velocidade base com um número aleatório entre 0 e 7.  
O tempo de espera entre os avanços é proporcional à sua velocidade.  
Ao atingir a linha de chegada (100 metros), o cavalo entra numa região crítica protegida por mutex para atualizar a colocação final.  
Sincronização com Mutex  

### Problema abordado:  

Sem sincronização, múltiplas threads poderiam alterar a variável colocacao_atual simultaneamente, resultando em dados inconsistentes ou   repetições de colocação.
Solução aplicada:

Foi utilizado um mutex global para proteger a região onde:

A colocação é incrementada e atribuída ao cavalo que chegou.
O primeiro cavalo a completar a corrida define o vencedor da aposta.
Resultado:

O uso do mutex garante integridade nas operações entre as threads.
A colocação dos cavalos e o vencedor da corrida são sempre definidos corretamente.
Apostas

Antes da corrida:

O jogador realiza uma aposta selecionando um número de cavalo entre 1 e 6.
Após a corrida:

O programa compara o cavalo apostado com o vencedor real.
Mostra a classificação final da corrida e informa se o jogador venceu ou perdeu a aposta.
Exemplo de Execução

🎰 BEM-VINDO AO HIPISMO DIGITAL! 🎰

🐎 CAVALOS DISPONÍVEIS PARA APOSTA: 1 - Relâmpago 2 - Pé de Pano 3 - Trovão 4 - Estrela Cadente 5 - Furacão 6 - Cometa

Em qual cavalo você aposta? (1 a 6): 3 ✅ Você apostou no Trovão! Boa sorte!

🏇 PREPARANDO A CORRIDA DE CAVALOS! ...

🐎 Relâmpago: 97 metros 🎉 Relâmpago terminou em 1º lugar! 🐎 Pé de Pano: 100 metros 🎉 Pé de Pano terminou em 2º lugar! ...

🎯 RESULTADO DA APOSTA: ❌ Que pena! Você errou. 😢 Trovão não venceu a corrida. ⭐ O vencedor foi Relâmpago

### Conclusão

A utilização de threads permite simular de forma realista uma corrida paralela entre cavalos. No entanto, sem mecanismos de sincronização, como mutex, dados compartilhados como a colocação dos cavalos podem ser corrompidos.

Neste código, a exclusão mútua foi essencial para garantir que cada thread atualizasse os dados de forma segura, sem colisões, garantindo a precisão do resultado final e da aposta do jogador.

Como compilar e executar

O código foi testado em uma máquina virtual Linux (WSL Debian):

gcc -o corrida corrida.c -lpthread ./corrida

## questao 2
## Questão 3  
Simular M contas bancárias acessadas por T threads, onde cada thread realiza transferências aleatórias entre contas.

- Sem controle, as threads podem gerar condições de corrida, alterando a soma total de dinheiro.  
- Usando mutexes, a soma total deve ser preservada.
- O programa compara as duas situações (sem trava e com trava).

## Explicação do codigo
### inicio
- Criamos um vetor de contas, cada uma iniciada com um saldo fixo.
- Cada conta possui um mutex próprio, para controlar acessos concorrentes.
### Execução das threads
- Cada thread executa várias transferências.
- Em cada transferência, escolhe uma conta de origem, uma de destino e um valor aleatório.
- A origem perde o valor e a conta de destino recebe o mesmo valor.
### Versão sem trava
- As threads modificam as contas diretamente, sem usar mutex.
- Como resultado, podem ocorrer condições de corrida, onde dois acessos simultâneos sobrescrevem saldos incorretamente.
- Isso faz a soma total do dinheiro variar, “criando” ou “perdendo” dinheiro.
### Versão com trava
- Antes de alterar as contas, as threads travam os mutexes correspondentes.
- A ordem de travamento é sempre a mesma (primeiro a conta de índice menor, depois a maior), para evitar deadlock.
- Após a operação, os mutexes são liberados.
- Dessa forma, nenhuma operação se perde e a soma total se mantém constante.
### Verificação do invariante
- O programa calcula a soma inicial e a soma final das contas.
- No final da execução com mutex, há um assert que garante que a soma final seja igual à inicial
### Como Compilar e Executar
usei maquina virtual linux wsl debian

- gcc -o transf exer3.c
- ./transf
## exemplos de saida
peixoto@LAPTOP-SIFK91NF:~/so2/Pthreads$ ./transf
Sem trava: soma inicial = 10000, soma final = 63039
Com trava: soma inicial = 10000, soma final = 10000

- Sem trava: a soma final varia a cada execução, podendo ser maior ou menor que a soma inicial.
- Com trava: a soma final é sempre idêntica à inicial, mostrando que a sincronização foi bem-sucedida.
## conclusão
concluimos que que a sicronização e essencial em sistemas concorrentes.com mutex garantimos exclusao mutua, ja sem mutex ocorre condicao de corrida onde corrompem os dados.

## Questão 4
Construção de uma linha de processamento com três threads (captura, processamento e gravação) conectadas por filas limitadas e protegidas por mutex e variáveis de condição.

### Como Compilar e Executar
usei maquina virtual linux wsl debian

- gcc -o proc exer4.c
- ./proc
### Como a Sincronização Funciona
Para evitar que as threads "se atropelem" ao usar as filas, usamos duas ferramentas:

- Mutex (O Cadeado): Cada fila tem um "cadeado". Antes de mexer na fila, uma thread precisa pegar esse cadeado. Isso garante que só uma thread por vez acesse a fila, evitando bagunça e perda de dados.

- Variáveis de Condição (Os Sinais): Elas evitam que as threads gastem energia à toa.Se uma thread quer adicionar um item e a fila está cheia, ela dorme até receber o sinal de que um espaço foi liberado.Se uma thread quer remover um item e a fila está vazia, ela dorme até receber o sinal de que um item novo chegou.

## Execução
peixoto@LAPTOP-SIFK91NF:~/so2/Pthreads$ ./proc
Iniciando pipeline...
[CAPTURA] Gerou: 0
[CAPTURA] Gerou: 1
[CAPTURA] Gerou: 2
[CAPTURA] Gerou: 3
[CAPTURA] Gerou: 4
[CAPTURA] Gerou: 5
[CAPTURA] Gerou: 6
[PROC] Processou 0 -> 0
[PROC] Processou 1 -> 2
[CAPTURA] Gerou: 7
[GRAVACAO] Gravou item final: 0
[CAPTURA] Gerou: 8
[GRAVACAO] Gravou item final: 2
[PROC] Processou 2 -> 4
[GRAVACAO] Gravou item final: 4
[PROC] Processou 3 -> 6
[CAPTURA] Gerou: 9
[CAPTURA] Gerou: 10
[GRAVACAO] Gravou item final: 6
[PROC] Processou 4 -> 8
[GRAVACAO] Gravou item final: 8
[PROC] Processou 5 -> 10
[CAPTURA] Gerou: 11
[CAPTURA] Gerou: 12
[GRAVACAO] Gravou item final: 10
[PROC] Processou 6 -> 12
[GRAVACAO] Gravou item final: 12
[PROC] Processou 7 -> 14
[CAPTURA] Gerou: 13
[CAPTURA] Gerou: 14
[GRAVACAO] Gravou item final: 14
[PROC] Processou 8 -> 16
[GRAVACAO] Gravou item final: 16
[PROC] Processou 9 -> 18
[PROC] Processou 10 -> 20
[PROC] Processou 11 -> 22
[PROC] Processou 12 -> 24
[PROC] Processou 13 -> 26
[PROC] Processou 14 -> 28
[GRAVACAO] Gravou item final: 18
[GRAVACAO] Gravou item final: 20
[GRAVACAO] Gravou item final: 22
[GRAVACAO] Gravou item final: 24
[GRAVACAO] Gravou item final: 26
[GRAVACAO] Gravou item final: 28
[CAPTURA] Enviou SINAL_FIM.
[PROCESSAMENTO] Recebeu SINAL_FIM. Repassando.
[GRAVACAO] Recebeu SINAL_FIM. Encerrando.
Pipeline finalizado.

## Análise do Resultado
Tudo foi processado: Nenhum item foi perdido. Todos os 15 itens gerados passaram pelos 3 estágios.
A ordem está correta: Para cada item, a ordem foi sempre Captura -> Processamento -> Gravação.
O programa terminou certo: O SINAL_FIM foi passado de thread em thread, garantindo que o programa encerrasse de forma limpa e sem travar.

## questao 5
O programa cria N_THREADS (por padrão, 4).
Existe uma fila  de tarefas (taskQueue) com capacidade MAX_TASKS.
A thread principal enfileira tarefas (ex.: n = 0 a 10) chamando addTask(n).
As threads  filhas ficam bloqueadas esperando tarefas; quando há uma trabalhando, elas:
retiram a tarefa da fila com exclusão mútua;
liberam o lock;
executam a função Fibonacci;
imprimem o resultado.
O processo se repete indefinidamente, até encerrar 

## Questão 7
Relatório – Problema dos Filósofos: Prevenção de Deadlock com Lock Ordenado e Semáforo

Objetivo

Simular o clássico problema dos filósofos (Dining Philosophers Problem), onde N filósofos compartilham N garfos (recursos) e devem alternar entre pensar e comer. Cada filósofo só pode comer se tiver os dois garfos adjacentes disponíveis.

A simulação é feita por meio de múltiplas threads Python, e o objetivo principal é comparar duas estratégias diferentes de prevenção de deadlock:

Versão A: Uso de ordenação global dos locks (lock de menor índice primeiro).
Versão B: Uso de um semáforo (porta) que limita o número de filósofos tentando comer simultaneamente.
Em ambas as versões, também é aplicada uma lógica de mitigação de fome (starvation) para evitar que algum filósofo coma repetidamente enquanto outros aguardam.

Configurações do Código

Parâmetros definidos no início do código:

N: número de filósofos e de garfos (ex: 5).
DURACAO_SEGUNDOS: tempo total da simulação (ex: 10s).
RELATAR_EVENTOS: define se imprime início/fim de cada refeição (opcional, para debug).
Métricas ao final:

Número de refeições feitas por cada filósofo.
Tempo máximo de espera de cada filósofo para começar a comer.
Versão A – Lock com Ordem Global

Descrição da abordagem:

Cada filósofo tenta pegar os dois garfos adjacentes.
Antes de tentar os locks, a ordem dos índices dos garfos é organizada: sempre tenta pegar primeiro o de menor índice, depois o de maior.
Isso garante uma ordem total de alocação dos recursos, evitando situações de espera circular (causa de deadlock).
Mitigação de fome:

Um filósofo só tenta comer se ele tiver feito até no máximo uma refeição a mais que o filósofo que menos comeu.
Resumo:

Previne deadlock pela ordenação na aquisição dos locks.
Simples de implementar, eficiente em pequenos grupos.
Versão B – Semáforo como Porta Global

Descrição da abordagem:

Um semáforo com valor N-1 (ex: 4 para 5 filósofos) é usado como “portão de entrada”.
Só permite que no máximo N-1 filósofos tentem pegar garfos ao mesmo tempo.
Isso garante que sempre haja pelo menos um filósofo com acesso exclusivo a um garfo que os outros precisam, evitando o ciclo de espera circular.
Funcionamento:

Antes de tentar os locks, o filósofo precisa adquirir o semáforo.
Após comer (ou se não conseguir os locks), libera o semáforo.
Mitigação de fome:

Igual à versão A, verifica se está comendo muito mais que os demais.
Resumo:

Previne deadlock pelo controle de concorrência via semáforo.
Útil quando não se quer ou não se pode impor uma ordem global de locks.
Exemplo de Saída (modo silencioso):

===== RESUMO FINAL (ORDEM) ===== Tempo: 10s

Filósofo 0: refeições= 40 maior_espera= 5.2 ms Filósofo 1: refeições= 39 maior_espera= 4.9 ms Filósofo 2: refeições= 39 maior_espera= 5.1 ms Filósofo 3: refeições= 40 maior_espera= 4.8 ms Filósofo 4: refeições= 39 maior_espera= 5.0 ms

===== RESUMO FINAL (SEMAFORO) ===== Tempo: 10s

Filósofo 0: refeições= 41 maior_espera= 6.3 ms Filósofo 1: refeições= 40 maior_espera= 6.0 ms Filósofo 2: refeições= 39 maior_espera= 5.9 ms Filósofo 3: refeições= 40 maior_espera= 6.1 ms Filósofo 4: refeições= 40 maior_espera= 6.0 ms

Comparação entre as versões

Critério	Versão A (Ordem de Lock)	Versão B (Semáforo)
Prevenção de Deadlock	Ordem total de locks	Limita concorrência com semáforo
Flexibilidade	Baixa (requer ordem global)	Alta (pode manter ordem local)
Complexidade	Simples	Levemente maior
Mitigação de fome	Simples e igual em ambas	Simples e igual em ambas
Conclusão

Ambas as abordagens são eficazes na prevenção de deadlock no problema dos filósofos. A versão com ordenação global é mais simples, mas depende de garantir uma ordem fixa nos recursos. Já a versão com semáforo é mais flexível e pode ser útil em sistemas mais dinâmicos.

Além disso, o uso da mitigação de fome é fundamental para manter a equidade entre os filósofos e garantir que todos consigam comer com frequência semelhante.

Esse problema é uma excelente representação de desafios reais em sistemas concorrentes, como escalonadores de threads, bancos de dados e controle de acesso a dispositivos.

Execução

Ambos os códigos são escritos em Python e não requerem bibliotecas externas além da padrão. Para executar:

python3 filosofos_ordem.py python3 filosofos_semaforo.py
## questao 8
## Questão 9
Relatório – Corrida com Barreira de Threads (pthread_barrier_t)

Objetivo

Simular uma corrida composta por várias etapas (pernas), onde múltiplas threads (corredores) devem sincronizar-se a cada etapa para simular uma volta completa.

O programa mede quantas voltas completas são realizadas em um determinado tempo (10 segundos), sincronizando os corredores usando barreiras (pthread_barrier_t) para garantir que todos avancem juntos para a próxima etapa.

Estrutura do Código

Parâmetros definidos:

K_THREADS: número de corredores (threads).
LEGS_POR_VOLTA: número de etapas (pernas) que compõem uma volta.
DURACAO_SEG: tempo total de execução da simulação (em segundos).
Funções principais:

agora_ns(): retorna o tempo atual em nanossegundos, usado para controle de duração.
corredor(): função executada por cada thread, responsável por simular o avanço do corredor em cada etapa e sincronizar com os demais.
Execução das Threads

Cada thread representa um corredor.

Em um laço de repetição:

A thread "descansa" um tempo aleatório (simulando esforço da etapa) usando nanosleep.
Após esse tempo, todas as threads se encontram em uma barreira (pthread_barrier_wait).
Quando todas chegam na barreira, elas são liberadas simultaneamente para a próxima etapa.
Apenas uma thread (retorno PTHREAD_BARRIER_SERIAL_THREAD) atualiza a etapa (perna) global.
Quando o número de etapas atinge o total de pernas por volta, é registrada a conclusão de uma volta e o contador global de voltas (voltas) é incrementado.
Controle de tempo

A função main inicia a medição de tempo e espera até atingir a duração de execução especificada (10 segundos).
Durante esse tempo, as threads continuam se sincronizando e registrando voltas.
Ao final do tempo, o flag parar é ativado e todas as threads terminam seu laço.
Sincronização com Barreira

Problema abordado:

Em sistemas concorrentes, sem sincronização, threads podem avançar em velocidades diferentes, tornando difícil identificar quando uma volta completa foi feita.
Solução aplicada:

Utiliza-se pthread_barrier_t para garantir que todas as threads avancem juntas para a próxima etapa.
A barreira age como um ponto de encontro: nenhuma thread avança até que todas tenham chegado.
Isso garante que uma volta só seja contabilizada após todos os corredores completarem as etapas simultaneamente.
Exemplo de Saída

Iniciando corrida: K=4, pernas/volta=4, duracao=10s Volta concluida #1 Volta concluida #2 Volta concluida #3 Volta concluida #4 ... Voltas totais: 13 Taxa: 78.00 voltas/min

Conclusão

Esse programa demonstra o uso eficaz de barreiras (pthread_barrier_t) para sincronizar múltiplas threads em um ponto comum. Ele mostra como é possível controlar fluxos paralelos para realizar uma ação conjunta, como contabilizar voltas em uma corrida.

O uso da barreira elimina a necessidade de mutex para contagem de voltas, pois apenas uma thread executa a atualização da variável voltas a cada barreira.

Como Compilar e Executar

Esse código pode ser compilado e testado em sistemas Linux, incluindo máquinas virtuais ou WSL (Windows Subsystem for Linux). Exemplo de comandos:

gcc -o corrida_barreira corrida_barreira.c -lpthread ./corrida_barreira

## QUESTÃO 10
Relatório – Simulação de Deadlock com Threads, Mutex e Detecção via Watchdog

Objetivo

Este programa tem como objetivo simular um ambiente multithread com múltiplos recursos compartilhados. Cada thread representa um processo que tenta adquirir dois recursos (mutexes). O objetivo é comparar dois cenários:

Um com ordem forçada de aquisição que pode gerar deadlock;
Outro com ordem total que evita deadlock.
Além disso, o programa conta com uma thread “watchdog” que monitora o progresso do sistema e detecta situações de deadlock.

Configurações Principais

As opções de configuração do programa são definidas por meio de parâmetros de linha de comando:

-m: modo de alocação ("bad" para forçar deadlock, ou omitir para evitar).
-n: número de recursos (e de threads, pois há uma thread por recurso).
-T: tempo de espera máximo sem progresso para detectar deadlock.
-d: duração da simulação no modo sem deadlock.
Exemplo de execução: ./programa -m bad -n 5 -T 2 Executa com 5 threads, modo que permite deadlock, com timeout de 2 segundos.

Estrutura do Código

Cada recurso é representado por um pthread_mutex_t. Há:

Um vetor de mutexes (resource_lock);
Vetores auxiliares que guardam informações para debug: qual thread possui qual recurso (owned_by), qual recurso cada thread está tentando adquirir (waiting_resource), e em qual fase da execução cada thread se encontra (thread_phase).
Execução das Threads

Cada thread executa a função worker_main. O comportamento varia conforme o modo escolhido:

Modo BAD (com possível deadlock):

Cada thread tenta adquirir dois recursos em ordem circular (ex: thread 0 tenta pegar recurso 0 e depois 1).
Como todas as threads fazem isso simultaneamente, há uma chance de todas se bloquearem na segunda aquisição, gerando um deadlock clássico do tipo "circular wait".
Modo GOOD (sem deadlock):

Os recursos são sempre adquiridos em ordem crescente (menor índice primeiro).
Isso impede que ocorra espera circular, eliminando a possibilidade de deadlock.
Etapas da execução de cada thread:

Marca que está aguardando o primeiro recurso.
Tenta adquirir o primeiro mutex.
Aguarda brevemente (usleep) para simular tempo de uso.
Marca que está aguardando o segundo recurso.
Tenta adquirir o segundo mutex.
Realiza trabalho por um curto tempo.
Atualiza o tempo do último progresso.
Libera os recursos.
Watchdog

A função watchdog_main é executada em uma thread separada.
Verifica periodicamente se houve progresso nas threads.
Se o tempo desde o último progresso ultrapassar o timeout_s, assume que houve deadlock.
Em caso de deadlock, imprime um snapshot do estado das threads e recursos, e encerra o programa com _exit.
Relatório de Estado em caso de Deadlock

Exemplo de saída no modo BAD com deadlock:

=== Watchdog: 2 s sem progresso === Recursos: R0 -> T0 R1 -> T1 R2 -> T2 R3 -> T3 R4 -> T4 Threads: T0: wait2 aguardando R1 T1: wait2 aguardando R2 T2: wait2 aguardando R3 T3: wait2 aguardando R4 T4: wait2 aguardando R0 Conclusao: deadlock detectado.

Esse relatório mostra que cada thread está aguardando por um recurso que está sendo segurado por outra, formando um ciclo de espera, caracterizando um deadlock.

Modo GOOD

No modo GOOD, a execução termina após um tempo pré-definido (run_s) e a mensagem final é exibida:

Execucao concluida sem deadlock.

Conclusão

Esse programa demonstra de forma prática o conceito de deadlock em sistemas concorrentes, causado por:

Espera circular;
Alocação parcial de recursos;
Não preempção.
E mostra também como evitá-lo impondo uma ordem total de aquisição dos recursos.

Além disso, o uso de uma thread watchdog permite a detecção ativa de deadlocks com diagnóstico completo do estado do sistema, útil para depuração e análise de sistemas reais.

Compilação e Execução

gcc -o deadlock_sim deadlock_sim.c -lpthread ./deadlock_sim -m bad -n 5 -T 2

ou

./deadlock_sim -m good -n 5 -T 2 -d 8