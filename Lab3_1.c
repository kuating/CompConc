#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

long long int dim; //numero de elementos
int nthreads; //numero de threads

//fluxo das threads
void * tarefa (void * arg){
	long int id = (long int) arg; //identificador da thread
	double *somaLocal = 0; //variavel local da soma de elementos
	long long int tamBloco = dim / nthreads; //tamanho do bloco de cada thread
	long int ini = id * tamBloco; //elemento inicial do bloco da thread
	long int fim; //elemento final(nao processado) do bloco
	
	somaLocal = (double*)malloc(sizeof(double));
	if(somaLocal == NULL){
		fprintf(stderr,"ERRO--malloc\n");
		exit(1);
	}
	*somaLocal = 0;
	if(id == nthreads-1) fim = dim; //Se for ultima thread, ir ao final
	else fim = ini + tamBloco; //Se nao, nao vai
	
	for(long long int i = ini; i<fim; i++){
		*somaLocal += (i%2==0) ? 4.0/(1+2*i) : -4.0/(1+2*i); //ternario para alternar sinais
	}
	
	//retorna o resultado
	pthread_exit((void *) somaLocal);
}

//fluxo principal
int main(int argc, char *argv[]){
	
	
	double somaSeq = 0; //soma sequencial
	double somaConc = 0; //soma concorrente
	double ini,fim; //tomada de tempo
	pthread_t* tid; //identificadores das threads no sistema
	double *retorno; //valor de retorno das threads
	
	double aceleracao;
	
	//recebe e valida os parametros de entrada
	if(argc < 3){
		fprintf(stderr, "Digite: %s <numero de elementos> <numero threads>\n", argv[0]);
		return 1;
	}
	dim = atoll(argv[1]);
	nthreads = atoi(argv[2]);
	
	GET_TIME(ini);
	//soma sequencial dos elementos
	for(long long int i = 0; i<dim; i++){
		somaSeq += (i%2==0) ? 4.0/(1+2*i) : -4.0/(1+2*i); //ternario para alternar sinais
	}
	GET_TIME(fim);
	printf("Tempo sequencial: %lf\n", fim-ini);
	
	aceleracao = fim-ini;
	
	//soma concorrente dos elementos
	GET_TIME(ini);
	
	tid = (pthread_t *)malloc(sizeof(pthread_t)*nthreads);
	if(tid==NULL){
		fprintf(stderr,"ERRO--malloc\n");
		return 2;
	}
	//criar as threads
	for(long int i=0; i<nthreads; i++){
		if(pthread_create(tid+i, NULL, tarefa, (void*) i)){
			fprintf(stderr,"ERRO--pthread_create\n");
			return 3;
		}
	}
	
	//aguardar o termino das threads
	for(long int i=0; i<nthreads; i++){
		if(pthread_join(*(tid+i), (void**) &retorno)){
			fprintf(stderr,"ERRO--pthread_join\n");
			return 4;
		}
		somaConc += *retorno;
		
	}
	
	GET_TIME(fim);
	printf("Tempo concorrent: %lf\n", fim-ini);
	
	aceleracao -= fim-ini;
	printf("Aceleracao: %lf\n", aceleracao);
	
	//exibir resultados
	printf("Soma sequencial: %.15lf %.15lf\n", somaSeq, somaSeq-M_PI);
	printf("Soma concorrent: %.15lf %.15lf\n", somaConc, somaConc-M_PI);
	
	//libera as areas de memoria alocadas
	free(retorno);
	free(tid);
	
	return 0;
}
