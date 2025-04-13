/**		Rômulo Silva de Oliveira

		Programação Concorrente com Pthreads e Linguagem C

		s12-a57-buffercirc.h

*/


#ifndef		LISTAPORTAS_H
#define		LISTAPORTAS_H



// Número de dígitos no código da porta
#define	TAM_PORTA		3

// Número de dígitos no código do usuário
#define	TAM_USUARIO		7	



/** Inicializa o monitor
*/
extern void listaportas_init(void);


/** Cadastra 'usuario' como autorizado a acessar 'porta'.
	Os strings precisam ser copiados.
	Caso 'usuario' já esteja autorizado para 'porta', nada muda.
	Esta rotina será chamada eventualmente por threads do tipo escritor.
	Retorna -1 caso o monitor não tenha sido inicializado ou parâmetros incorretos.
*/
extern int listaportas_insere( char *porta, char *usuario);


/** Indaga se 'usuario' está autorizado a acessar 'porta'.
	Retorna 1 para autorizado e 0 para não autorizado.
	Retorna -1 caso o monitor não tenha sido inicializado ou parâmetros incorretos.
	Esta rotina será chamada frequentemente por threads do tipo leitor.
*/
extern int listaportas_consulta( char *porta, char *usuario);
		

/** Retorna o número de usuários cadastrados
*/
extern int listaportas_cadastrados(void);



#endif		// LISTAPORTAS_H



