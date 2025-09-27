#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N_THREADS 4
#define MAX_TASKS 20

typedef struct
{
  int n;
} Task;

Task taskQueue[MAX_TASKS];
int front = 0, rear = 0, count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

long long fibonacci(int n)
{
  if (n <= 1)
    return n;
  return fibonacci(n - 1) + fibonacci(n - 2);
}

void *worker(void *arg)
{
  while (1)
  {
    Task task;

    pthread_mutex_lock(&mutex);
    while (count == 0)
    {
      pthread_cond_wait(&cond, &mutex);
    }
    task = taskQueue[front];
    front = (front + 1) % MAX_TASKS;
    count--;
    pthread_mutex_unlock(&mutex);

    printf("Thread %ld processando Fibonacci(%d)...\n", pthread_self(), task.n);
    long long result = fibonacci(task.n);
    printf("Resultado Fibonacci(%d) = %lld\n", task.n, result);
  }
  return NULL;
}

void addTask(int n)
{
  pthread_mutex_lock(&mutex);
  if (count < MAX_TASKS)
  {
    taskQueue[rear].n = n;
    rear = (rear + 1) % MAX_TASKS;
    count++;
    pthread_cond_signal(&cond);
  }
  pthread_mutex_unlock(&mutex);
}

int main()
{
  pthread_t threads[N_THREADS];

  for (int i = 0; i < N_THREADS; i++)
  {
    pthread_create(&threads[i], NULL, worker, NULL);
  }

  for (int i = 0; i < 10; i++)
  {
    addTask(i);
    sleep(1);
  }

  return 0;
}