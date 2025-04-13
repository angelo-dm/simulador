#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define MAX_SIZE 5

pthread_t threads[MAX_SIZE];

int SUCESS_T = 222, ERROR_T = 444;


void codigo_tarefa(char *name) {
    for (int i = 0; i < 10; i++) {
        sleep(1);
        printf("%s: passaram %d segundos\n", name, i + 1);
    }

    if ((int)name[7] % 2 == 0) {
        pthread_exit(&SUCESS_T);
    } else {
        pthread_exit(&ERROR_T);
    }
}

int main() {
    char names[][15] = {"Tarefa 1", "Tarefa 2", "Tarefa 3", "Tarefa 4", "Tarefa 5"};
    int *retorno[MAX_SIZE];

    printf("inicio\n");

    for (int i = 0; i < MAX_SIZE; i++) {
        pthread_create(&threads[i], NULL, (void *) codigo_tarefa, (void *) &names[i]);
    }

    for (int j = 0; j < MAX_SIZE; j++) {
        pthread_join(threads[j], (void **) &retorno[j]);
        printf("A thread %s retornou %d\n", names[j], *retorno[j]);

    }

    printf("fim");
    return 0;
}
