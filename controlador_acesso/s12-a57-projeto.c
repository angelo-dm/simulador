/**		Rômulo Silva de Oliveira

		Programação Concorrente com Pthreads e Linguagem C

		s12-a57-projeto.c

		Crie um programa que faz o controle de acesso a um conjunto de portas.
		
		O prédio possui cerca de 100 portas, identificadas por números de 3 dígitos.
		
		Cada porta possui uma lista de usuários autorizados, cada usuário é
		identificado por um código de 7 dígitos.
		
		Crie um monitor 's12-a57-listaportas' que implementa uma lista de portas e,
		para cada porta, uma lista de usuários autorizados. Use a estrutura de dados que
		lhe for mais confortável. Precisa ser um monitor pois esta estrutura de dados 
		será acessada concorrentemente por várias threads.
		
		Sugestão de rotinas de acesso :
		
		/ ** Cadastra 'usuario' como autorizado a acessar 'porta'.
			Os strings precisam ser copiados.
			Caso 'usuario' já esteja autorizado para 'porta', nada muda.
			Esta rotina será chamada eventualmente por threads do tipo escritor.
			Retorna -1 caso o monitor não tenha sido inicializado ou parâmetros incorretos.
		* /
		extern int listaportas_insere( char *porta, char *usuario);


		/ ** Indaga se 'usuario' está autorizado a acessar 'porta'.
			Retorna 1 para autorizado e 0 para não autorizado.
			Retorna -1 caso o monitor não tenha sido inicializado ou parâmetros incorretos.
			Esta rotina será chamada frequentemente por threads do tipo leitor.
		* /
		extern int listaportas_consulta( char *porta, char *usuario);


		Dado o elevado número de portas será utilizado um pool de threads para processar
		as consultas. Crie um monitor 's12-a57-threadpool' adaptando o monitor threadpool
		descrito na aula 's11-a51-threadpool'.		

		No módulo principal crie uma thread que simula o cadastramento de usuários, gerando
		aleatoriamente códigos de porta e usuário e inserindo a informação na lista de portas.

		No módulo principal, crie uma thread para simular a recepção pela rede de consultas 
		sobre autorizações de usuários. Esta thread deve gerar aleatoriamente códigos de usuários
		e portas e inserir o trabalho de consulta na fila de trabalhos do threadpool. O resultado
		(autorizado ou não) deve ser escrito na tela como parte do trabalho.
		
		Trabalhos não são descartados. Caso a fila de trabalhos esteja lotada, esta thread fica 
		bloqueada até conseguir inserir o novo trabalho na fila.

		No módulo principal, inicialize o threadpool com 10 threads para realizar o trabalho
		de consultar as listas de portas.

		O programa é terminado quando o usuário teclar <enter>.
*/


#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include "s12-a57-listaportas.h"
#include "s12-a57-threadpool.h"


#define	N_THREAD_POOL	10
#define	MAX_TRABALHOS	1000

#define N_CADASTROS		100000


/** Thread que simula o cadastramento de usuários,
	gerando aleatoriamente códigos de porta e usuário
	e inserindo a informação na lista de portas
	
	Pode gerar cadastros repetidos, são filtrados por listaportas_insere()
*/
static void codigo_tarefa_cadastra(void) {
	char porta[TAM_PORTA+1];
	char usuario[TAM_USUARIO+1];

	porta[TAM_PORTA] = '\0';
	usuario[TAM_USUARIO] = '\0';

	for( int i=0; i<N_CADASTROS; ++i) {
		for( int j=0; j<TAM_PORTA; ++j)
			porta[j] = '0' + rand()%10;
		for( int j=0; j<4; ++j)
			usuario[j] = '0';
		for( int j=4; j<TAM_USUARIO; ++j)
			usuario[j] = '0' + rand()%10;
			
		if( listaportas_insere(porta,usuario) == -1 ) {
			printf("### Erro inserindo no cadastro\n");
			exit(EXIT_FAILURE);
		}
	}
}



/**	Função que corresponde ao trabalho a ser executado
*/
static void fazConsulta( char *porta, char *usuario) {
	if( listaportas_consulta( porta, usuario) == 1 )
		printf("Thread %lu Porta %s Usuario %s autorizado\n", pthread_self(), porta, usuario);
	else
		printf("Thread %lu Porta %s Usuario %s NAO autorizado\n", pthread_self(), porta, usuario);
		
	// Faltou free de porta e usuario !!!
}




/** Thread que simula o recebimento de consultas pela rede
	Memória alocada na criação da tarefa será liberada na execução da tarefa
*/
static void codigo_tarefa_consultas(void) {
	while( 1 ) {
		char *porta = calloc( TAM_PORTA+1, sizeof(char));
		char *usuario = calloc( TAM_USUARIO+1, sizeof(char));

		porta[TAM_PORTA] = '\0';
		usuario[TAM_USUARIO] = '\0';

		for( int j=0; j<TAM_PORTA; ++j)
			porta[j] = '0' + rand()%10;
		for( int j=0; j<4; ++j)
			usuario[j] = '0';
		for( int j=4; j<TAM_USUARIO; ++j)
			usuario[j] = '0' + rand()%10;
		threadpool_insereTrabalho( fazConsulta, porta, usuario);
		usleep(500);
	}		
}




/** Função principal, cria o thread pool e usa ele
*/
int main(void){
	pthread_t th_cadastra;
	pthread_t th_consulta;
	
	printf("Inicio\n");
	srand(time(NULL));

	// Inicializa a lista de autorizações
	listaportas_init();

	// Teste simples da lista de autorizações
	if( listaportas_insere("000","0000000") == -1 ) {
		printf("### Erro inserindo no cadastro\n");
		exit(EXIT_FAILURE);
	}
	if( listaportas_insere("001","0000001") == -1 ) {
		printf("### Erro inserindo no cadastro\n");
		exit(EXIT_FAILURE);
	}
	if( listaportas_insere("000","0000000") == -1 ) {
		printf("### Erro inserindo no cadastro\n");
		exit(EXIT_FAILURE);
	}
	if( listaportas_consulta("000","0000000") != 1		||
		listaportas_consulta("001","0000001") != 1		||
		listaportas_consulta("002","0000002") != 0		) {
			printf("### Erro consultando o cadastro\n");
			exit(EXIT_FAILURE);
	}
	printf("Digite <enter> para iniciar e depois <enter> para terminar:\n");
	getchar();


	// Cria o thread pool
	if( threadpool_init( N_THREAD_POOL, MAX_TRABALHOS) == -1 ) {
		printf("Não conseguiu criar o thread pool\n");
		exit(EXIT_FAILURE);
	}

	// Cria thread que faz os cadastramentos
	pthread_create(&th_cadastra, NULL, (void *)codigo_tarefa_cadastra, NULL);

	// Cria thread que recebe os pedidos de consulta e solicita o trabalho
	pthread_create(&th_consulta, NULL, (void *)codigo_tarefa_consultas, NULL);

	// Espera o usuario teclar <enter>
	getchar();
	while( threadpool_trabalhosNaFila() > 0 )
		usleep(100);

	printf("Fim, foram cadastrados %d usuários\n", listaportas_cadastrados() );
	return(0);
}



