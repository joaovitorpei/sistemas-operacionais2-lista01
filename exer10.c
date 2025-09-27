#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

enum { MAX_RESOURCES = 64 };

typedef struct {
    int n_resources;
    int force_bad_order;
    int timeout_s;
    int run_s;
} settings_t;

static pthread_mutex_t resource_lock[MAX_RESOURCES];
static volatile int   owned_by[MAX_RESOURCES];
static volatile int   thread_phase[MAX_RESOURCES];
static volatile int   waiting_resource[MAX_RESOURCES];

static pthread_barrier_t start_gate;
static volatile long long last_progress;
static settings_t cfg = { .n_resources = 5, .force_bad_order = 1, .timeout_s = 2, .run_s = 8 };

static long long now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

static const char* phase_name(int s) {
    return s==0?"idle":s==1?"wait1":s==2?"wait2":s==3?"work":"?";
}

static void* worker_main(void* arg) {
    int tid = (int)(long)arg;
    int left  = tid % cfg.n_resources;
    int right = (tid + 1) % cfg.n_resources;

    pthread_barrier_wait(&start_gate);

    for (;;) {
        if (cfg.force_bad_order) {
            thread_phase[tid]   = 1; waiting_resource[tid] = left;
            pthread_mutex_lock(&resource_lock[left]);  owned_by[left]  = tid;
            usleep(100 * 1000);

            thread_phase[tid]   = 2; waiting_resource[tid] = right;
            pthread_mutex_lock(&resource_lock[right]); owned_by[right] = tid;
        } else {
            int a = left, b = right;
            if (a > b) { int tmp = a; a = b; b = tmp; }

            thread_phase[tid]   = 1; waiting_resource[tid] = a;
            pthread_mutex_lock(&resource_lock[a]); owned_by[a] = tid;
            usleep(1000);

            thread_phase[tid]   = 2; waiting_resource[tid] = b;
            pthread_mutex_lock(&resource_lock[b]); owned_by[b] = tid;
        }

        thread_phase[tid] = 3; waiting_resource[tid] = -1;
        usleep(50 * 1000);
        last_progress = now_ns();

        pthread_mutex_unlock(&resource_lock[right]); owned_by[right] = -1;
        pthread_mutex_unlock(&resource_lock[left]);  owned_by[left]  = -1;
        thread_phase[tid] = 0;
        usleep(20 * 1000);
    }
    return NULL;
}

static void print_snapshot_and_exit(void) {
    printf("\n=== Watchdog: %d s sem progresso ===\n", cfg.timeout_s);
    printf("Recursos:\n");
    for (int r = 0; r < cfg.n_resources; ++r) {
        if (owned_by[r] >= 0) printf("  R%-2d -> T%d\n", r, owned_by[r]);
        else                  printf("  R%-2d -> livre\n", r);
    }
    printf("Threads:\n");
    for (int t = 0; t < cfg.n_resources; ++t) {
        if (waiting_resource[t] >= 0)
            printf("  T%d: %s aguardando R%d\n", t, phase_name(thread_phase[t]), waiting_resource[t]);
        else
            printf("  T%d: %s\n", t, phase_name(thread_phase[t]));
    }
    printf("Conclusao: deadlock detectado.\n");
    fflush(stdout);
    _exit(2);
}

static void* watchdog_main(void* _) {
    (void)_;
    for (;;) {
        usleep(100 * 1000);
        if (now_ns() - last_progress > (long long)cfg.timeout_s * 1000000000LL)
            print_snapshot_and_exit();
    }
    return NULL;
}

static void parse_args(int argc, char** argv) {
    int opt;
    while ((opt = getopt(argc, argv, "m:n:T:d:")) != -1) {
        if (opt == 'm') cfg.force_bad_order = (strcmp(optarg, "bad") == 0);
        else if (opt == 'n') { cfg.n_resources = atoi(optarg);
        if (cfg.n_resources < 2) cfg.n_resources = 2;
        if (cfg.n_resources > MAX_RESOURCES) cfg.n_resources = MAX_RESOURCES; }
        else if (opt == 'T') { cfg.timeout_s = atoi(optarg);
        if (cfg.timeout_s < 1) cfg.timeout_s = 1; }
        else if (opt == 'd') { cfg.run_s = atoi(optarg);
        if (cfg.run_s < 1) cfg.run_s = 1; }
    }
}

int main(int argc, char** argv) {
    parse_args(argc, argv);

    for (int i = 0; i < cfg.n_resources; ++i) {
        pthread_mutex_init(&resource_lock[i], NULL);
        owned_by[i] = -1; thread_phase[i] = 0; waiting_resource[i] = -1;
    }
    pthread_barrier_init(&start_gate, NULL, cfg.n_resources);
    last_progress = now_ns();

    pthread_t thr[MAX_RESOURCES], guard;
    for (int i = 0; i < cfg.n_resources; ++i)
        pthread_create(&thr[i], NULL, worker_main, (void*)(long)i);
    pthread_create(&guard, NULL, watchdog_main, NULL);

    if (cfg.force_bad_order) {
        printf("Modo: BAD (deadlock forçado). N=%d, timeout=%ds\n", cfg.n_resources, cfg.timeout_s);
        fflush(stdout);
        pause();
    } else {
        printf("Modo: GOOD (ordem total). N=%d, timeout=%ds, duracao=%ds\n",
               cfg.n_resources, cfg.timeout_s, cfg.run_s);
        fflush(stdout);
        sleep(cfg.run_s);
        printf("Execucao concluida sem deadlock.\n");
    }
    return 0;
}