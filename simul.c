// simulador_concorrente: simulação de processamento concorrente e paralelismo

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define NUM_THREADS 4
#define NUM_TASKS 12

// ----------------------------- STRUCTS -----------------------------
typedef struct {
    int id;
    int duration;
    int priority;
    long start_time;
    long end_time;
    int assigned_core;
} Task;

typedef struct {
    int thread_id;
    int tasks_executed;
    long total_time;
} ThreadStats;

// ----------------------------- VARIÁVEIS GLOBAIS -----------------------------
Task task_queue[NUM_TASKS];
int current_task = 0;
pthread_mutex_t lock;
ThreadStats stats[NUM_THREADS];

// ----------------------------- FUNÇÕES AUXILIARES -----------------------------
long current_millis() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (ts.tv_sec * 1000L) + (ts.tv_nsec / 1000000L);
}

int compare_priority(const void* a, const void* b) {
    Task* t1 = (Task*)a;
    Task* t2 = (Task*)b;
    return t1->priority - t2->priority;
}

void execute_task(Task* task, int thread_id) {
    task->start_time = current_millis();
    task->assigned_core = thread_id;
    printf("[Core %d] Início da tarefa %d (prioridade %d)\n", thread_id, task->id, task->priority);
    usleep(task->duration * 1000);
    task->end_time = current_millis();
    printf("[Core %d] Fim da tarefa %d (real: %ld ms)\n", thread_id, task->id, task->end_time - task->start_time);

    stats[thread_id].tasks_executed++;
    stats[thread_id].total_time += (task->end_time - task->start_time);
}

// ----------------------------- WORKER THREAD -----------------------------
void* worker(void* arg) {
    int thread_id = *(int*)arg;
    while (1) {
        pthread_mutex_lock(&lock);
        if (current_task >= NUM_TASKS) {
            pthread_mutex_unlock(&lock);
            break;
        }
        Task* t = &task_queue[current_task++];
        pthread_mutex_unlock(&lock);

        execute_task(t, thread_id);
    }
    return NULL;
}

// ----------------------------- VISUALIZAÇÃO ESTILO GANTT -----------------------------
void mostrar_gantt() {
    printf("\n===== Gantt (Tarefa x Núcleo) =====\n");
    for (int i = 0; i < NUM_TASKS; i++) {
        printf("Tarefa %2d | Core %d | Início: %ld | Fim: %ld | Duração: %ld ms\n",
               task_queue[i].id,
               task_queue[i].assigned_core,
               task_queue[i].start_time,
               task_queue[i].end_time,
               task_queue[i].end_time - task_queue[i].start_time);
    }
}

// ----------------------------- MAIN -----------------------------
int main() {
    srand(time(NULL));
    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < NUM_TASKS; i++) {
        task_queue[i].id = i;
        task_queue[i].duration = 100 + rand() % 400;
        task_queue[i].priority = rand() % 5;
        task_queue[i].assigned_core = -1;
    }

    qsort(task_queue, NUM_TASKS, sizeof(Task), compare_priority);

    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    memset(stats, 0, sizeof(stats));

    long inicio_total = current_millis();

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        stats[i].thread_id = i;
        pthread_create(&threads[i], NULL, worker, &thread_ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    long fim_total = current_millis();

    mostrar_gantt();

    // Estatísticas por núcleo
    printf("\n===== Estatísticas =====\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        printf("Core %d: %d tarefas, tempo total: %ld ms, média: %.2f ms\n",
               i,
               stats[i].tasks_executed,
               stats[i].total_time,
               stats[i].tasks_executed ? (double)stats[i].total_time / stats[i].tasks_executed : 0.0);
    }

    printf("\nTempo total da simulação (real): %ld ms\n", fim_total - inicio_total);

    pthread_mutex_destroy(&lock);
    return 0;
}
