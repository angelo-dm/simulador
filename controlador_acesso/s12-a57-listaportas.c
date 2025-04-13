/**		Rômulo Silva de Oliveira

		Programação Concorrente com Pthreads e Linguagem C

		s12-a57-listaportas.c
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#include "s12-a57-listaportas.h"


// Usado para armazenar as portas
struct registro_porta_t {
	char porta[TAM_PORTA+1];				// Código de uma porta
	struct registro_porta_t *prox;			// Lista encadeada com as demais portas
	struct registro_usuario_t *usuarios;	// Lista de usuários desta porta
};		


// Usado para armazenar os usuários
struct registro_usuario_t {
	char usuario[TAM_USUARIO+1];		// Código de um usuário
	struct registro_usuario_t *prox;	// Lista encadeada com os demais usuários
};



static struct registro_porta_t *portas = NULL;

static int inicializado = 0;		// 1 indica que foi inicializado

static pthread_rwlock_t rw_lista = PTHREAD_RWLOCK_INITIALIZER;

static int nCadastrados = 0;		// Número de usuários cadastrados



/** Inicializa o monitor
	Capacidade informa quantos dados devem caber no buffer circular
*/
void listaportas_init(void) {
	inicializado = 1;
}



/** Cadastra 'usuario' como autorizado a acessar 'porta'.
	Os strings precisam ser copiados.
	Caso 'usuario' já esteja autorizado para 'porta', nada muda.
	Esta rotina será chamada eventualmente por threads do tipo escritor.
	Retorna -1 caso o monitor não tenha sido inicializado ou parâmetros incorretos.
*/
int listaportas_insere( char *porta, char *usuario) {
	struct registro_porta_t *pporta;
	struct registro_usuario_t *pusuario;

	if( inicializado == 0 )
		return -1;

	if( strlen(porta) != TAM_PORTA )
		return -1;
		
	if( strlen(usuario) != TAM_USUARIO )
		return -1;

	// Rotina usada por escritores
	pthread_rwlock_wrlock(&rw_lista);

	// Se porta ainda não existe precisa criar
	pporta = portas;
	while( pporta != NULL  &&  strcmp(pporta->porta,porta) != 0 )
		pporta = pporta->prox;

	if( pporta == NULL ) {
		// Não achou esta porta, vai criar
		pporta = malloc( sizeof(struct registro_porta_t) );
		if( pporta == NULL ) {
			// Alocação de memória falhou
			pthread_rwlock_unlock(&rw_lista);
			return -1;
		}
		else {
			// Insere na frente da lista de portas
			strcpy( pporta->porta, porta);
			pporta->prox = portas;
			pporta->usuarios = NULL;
			portas = pporta;
		}
	}

	// Na lista desta porta procura pelo usuario
	pusuario = pporta->usuarios;
	while( pusuario != NULL  &&  strcmp(pusuario->usuario,usuario) != 0 )
		pusuario = pusuario->prox;

	if( pusuario == NULL ) {
		// Não achou este usuario, vai criar
		pusuario = malloc( sizeof(struct registro_usuario_t) );
		if( pusuario == NULL ) {
			// Alocação de memória falhou
			pthread_rwlock_unlock(&rw_lista);
			return -1;
		}
		// Insere na frente da lista de usuarios desta porta
		strcpy( pusuario->usuario, usuario);
		pusuario->prox = pporta->usuarios;
		pporta->usuarios = pusuario;
		++nCadastrados;
	}

	// Sucesso
	pthread_rwlock_unlock(&rw_lista);
	return 0;
}	



/** Indaga se 'usuario' está autorizado a acessar 'porta'.
	Retorna 1 para autorizado e 0 para não autorizado.
	Retorna -1 caso o monitor não tenha sido inicializado ou parâmetros incorretos.
	Esta rotina será chamada frequentemente por threads do tipo leitor.
*/
int listaportas_consulta( char *porta, char *usuario) {
	struct registro_porta_t *pporta;
	struct registro_usuario_t *pusuario;

	if( inicializado == 0 )
		return -1;

	if( strlen(porta) != TAM_PORTA )
		return -1;
		
	if( strlen(usuario) != TAM_USUARIO )
		return -1;

	// Rotina usada por leitores
	pthread_rwlock_rdlock(&rw_lista);
	
	// Pesquisa as portas
	pporta = portas;
	while( pporta != NULL  &&  strcmp(pporta->porta,porta) != 0 )
		pporta = pporta->prox;
	if( pporta == NULL ) {
		// Porta não existe no cadastro
		pthread_rwlock_unlock(&rw_lista);
		return 0;
	}
	
	// Pesquisa usuários da porta
	pusuario = pporta->usuarios;
	while( pusuario != NULL  &&  strcmp(pusuario->usuario,usuario) != 0 )
		pusuario = pusuario->prox;
	if( pusuario == NULL ) {
		// Não achou este usuario
		pthread_rwlock_unlock(&rw_lista);
		return 0;
	}

	pthread_rwlock_unlock(&rw_lista);
	return 1;
}		


/** Retorna o número de usuários cadastrados
*/
int listaportas_cadastrados( void) {
	int ret;
	// Rotina usada por leitores
	pthread_rwlock_rdlock(&rw_lista);
	ret = nCadastrados;
	pthread_rwlock_unlock(&rw_lista);
	return ret;
}








