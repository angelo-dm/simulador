/**		Rômulo Silva de Oliveira

		Programação Concorrente com Pthreads e Linguagem C

		s01-a03-primeira.c
*/		


#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


pthread_t t1;
pthread_t t2;


void codigo_tarefa_1(void){
	for( int ns=1; ns < 10; ++ns) {
		sleep(1);
		printf("Tarefa 1: passaram %d segundos\n", ns);
	}
}


void codigo_tarefa_2(void){
	for( int ns=1; ns < 15; ++ns) {
		sleep(1);
		printf("Tarefa 2: passaram %d segundos\n", ns);
	}
}


int main(void){
		printf("Inicio\n");

        pthread_create(&t1, NULL, (void *) codigo_tarefa_1, NULL);
        pthread_create(&t2, NULL, (void *) codigo_tarefa_2, NULL);

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);

		printf("Fim\n");
		return 0;
}



