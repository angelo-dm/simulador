/**		Rômulo Silva de Oliveira

		Programação Concorrente com Pthreads e Linguagem C

		s12-a57-threadpool.h

*/


#ifndef		THREADPOOL_H
#define		THREADPOOL_H


/** Inicializa o monitor
	nThreads é o número de threads que formam o pool
	maxTrabalhos é o número máximod de trabalhos enfileirados
	Retorna 0 para OK e -1 em caso de falha
*/
extern int threadpool_init(int nThreads, int maxTrabalhos);


/** Insere um trabalho na fila
	Trabalho é chamar a funcao 'funcao' com os parâmetros 'valor1' e 'valor2'
	Uma melhor solução seria criar uma struct para passar todos os parâmetros da função
	No caso de fila de trabalhos lotada, fica bloqueado até ter lugar na fila de trabalhos
	Retorna 0 para OK e -1 para erro
*/
extern int threadpool_insereTrabalho(void(*funcao)(char *,char *), char *valor1, char *valor2);


/** Retorna o número de trabalhos na fila
*/
extern int threadpool_trabalhosNaFila(void);



#endif		// THREADPOOL_H



