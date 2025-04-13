/**		Rômulo Silva de Oliveira

		Programação Concorrente com Pthreads e Linguagem C

		s12-a57-threadpool.c
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#include "s12-a57-threadpool.h"



/* Fila de trabalhos */
struct trabalho_t {
	void (*funcao) (char *,char *);
	char *valor1;
	char *valor2;
};

static int pool_max_trabalhos = -1;
static int pool_nThreads = -1;

static struct trabalho_t **fila_trabalhos;	// Array de pointers
static int prox_insere = 0;
static int prox_retira = 0;
static int nTrabalhos = 0;

static pthread_mutex_t mutex_fila = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t tem_trabalho = PTHREAD_COND_INITIALIZER;
static pthread_cond_t tem_espaco = PTHREAD_COND_INITIALIZER;


/* Threads que formam o pool */
static pthread_t *th_pool;



/** Tarefa do pool de threads
	Retira um trabalho da fila e o executa
	Libera memória do descritor de trabalho
*/
static void codigo_tarefa_pool(void){
	struct trabalho_t *trab;
	printf("Thread criada %lu\n", pthread_self());
	while(1) {
		pthread_mutex_lock(&mutex_fila);
		while( nTrabalhos == 0) {
			pthread_cond_wait( &tem_trabalho, &mutex_fila);
		}
		printf("Pegando trabalho %d\n", prox_retira);
		trab = fila_trabalhos[prox_retira];
		prox_retira = (prox_retira + 1) % pool_max_trabalhos;
		--nTrabalhos;
		pthread_cond_signal(&tem_espaco);

		pthread_mutex_unlock(&mutex_fila);

		// Executa o trabalho, 'funcao' sabe o que fazer
		trab->funcao(trab->valor1, trab->valor2);

		// Libera memória deste descritor de trabalho
		free(trab);
    }
}




/** Inicializa o monitor
	nThreads é o número de threads que formam o pool
	maxTrabalhos é o número máximod de trabalhos enfileirados
	Retorna 0 para OK e -1 em caso de falha
*/
int threadpool_init(int nThreads, int maxTrabalhos) {
	int ret;
	
	pthread_mutex_lock(&mutex_fila);
	if( pool_max_trabalhos != -1  ||  pool_nThreads != -1 )
		ret = -1;
	else {
		pool_max_trabalhos = maxTrabalhos;
		pool_nThreads = nThreads;

		fila_trabalhos = calloc( pool_max_trabalhos, sizeof(struct trabalho_t *) );
		th_pool = calloc( nThreads, sizeof(pthread_t) );
		if( fila_trabalhos == NULL  ||  th_pool == NULL )
			ret = -1;

		for( int i=0; i<nThreads; ++i)
			pthread_create(&th_pool[i], NULL, (void *)codigo_tarefa_pool, NULL);
		ret = 0;
	}	
	pthread_mutex_unlock(&mutex_fila);
	return ret;
}	


/** Insere um trabalho na fila
	Trabalho é chamar a funcao 'funcao' com os parâmetros 'valor1' e 'valor2'
	Uma melhor solução seria criar uma struct para passar todos os parâmetros da função
	No caso de fila de trabalhos lotada, fica bloqueado até ter lugar na fila de trabalhos
	Retorna 0 para OK e -1 para erro
*/
int threadpool_insereTrabalho( void(*funcao)(char *,char *), char *valor1, char *valor2) {
	int ret;
	struct trabalho_t *novo;

	if( pool_nThreads == -1 )
		return -1;		// Não foi inicializado

	novo = malloc(sizeof(struct trabalho_t));
	if( novo == NULL )
		return -1;

	novo->funcao = funcao;
	novo->valor1 = valor1;	
	novo->valor2 = valor2;	
	
	pthread_mutex_lock(&mutex_fila);
	while( nTrabalhos == pool_max_trabalhos )
		pthread_cond_wait( &tem_espaco, &mutex_fila);

	//printf("Inserindo trabalho %d\n", prox_insere);
	fila_trabalhos[prox_insere] = novo;
	prox_insere = (prox_insere + 1) % pool_max_trabalhos;
	++nTrabalhos;
	pthread_cond_signal(&tem_trabalho);
	ret = 0;

	pthread_mutex_unlock(&mutex_fila);
	return ret;
}



/** Retorna o número de trabalhos na fila
*/
int threadpool_trabalhosNaFila( void) {
	int ret;
	pthread_mutex_lock(&mutex_fila);
	ret = nTrabalhos;
	pthread_mutex_unlock(&mutex_fila);
	return ret;
}



