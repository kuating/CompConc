#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

/* GERADOR DE MDC
 * Dados X e Y, gera minimo divisor comum ambos, de forma concorrente,
 * em dadas nthreads.
 * 
 * Entrada: <X> <Y> <nthreads>
 * Saida: mdc
 * 
 * OBS: entrada nao eh passada em argv
 *  */

long long int X; //primeiro numero
long long int Y; //segundo numero
int nthreads; //numero de threads
long long int mdc; //maximo divisor comum de X e Y
long long int MAXN; //raiz de min(X,Y). eh global para nao precisar
					//repetir a conta na thread ou passar como argumento

pthread_mutex_t lock; //variavel especial para sincronizacao de exclusao mutua

//fluxo das threads
void * tarefa (void * arg){
	long int id = (long int) arg; //identificador da thread
	
	//Pegar candidatos a mdc alternados decrescentes a partir de MAXN
	for(long long int n = MAXN-id; n>0; n-=nthreads){
		if(mdc > n) pthread_exit(NULL); //se ja encontrou mdc maior, fim
		if((X%n == 0) && (Y%n == 0)){ //se nao eh divisor comum 
			//inicio da secao critica
			pthread_mutex_lock(&lock);
			if(n>mdc) mdc=n;//maior divisor atual encontrado
			//fim da secao critica
			pthread_mutex_unlock(&lock);
			pthread_exit(NULL); //thread nao vai encontrar divisor maior
		}
	}
	pthread_exit(NULL);//nao encontrou divisor
}

//fluxo principal
int main(int argc, char *argv[]){
	
	double ini,fim; //tomada de tempo
	pthread_t* tid; //identificadores das threads no sistema
	
	//apresentacao
	printf("Este programa gera o MDC de X e Y de forma concorrente.\nInsira: <X> <Y> <numero_de_threads>\n");
	scanf("%lld %lld %d",&X,&Y,&nthreads);
	
	mdc = 1; //inicio mdc com seu valor minimo possivel
	if(X<=Y) MAXN = X/2; //maior divisor de X que nao ele mesmo eh no maximo X/2
	if(X>Y) MAXN = Y/2; //maior divisor de Y que nao ele mesmo eh no maximo Y/2
	//Se tiverem mais threads que divisores possiveis
	if(nthreads > MAXN) nthreads = MAXN; 
	
	if(X%Y==0) mdc=Y; //teste especifico (nao vale a pena paralelizar)
	else{
		if(Y%X==0) mdc=X; // teste especifico (nao vale a pena paralelizar)
		else{
			//tomada de tempo
			GET_TIME(ini);
			
			//inicializa a variavel de exclusao mutua
			pthread_mutex_init(&lock, NULL);
			
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
				if(pthread_join(*(tid+i), NULL)){
					fprintf(stderr,"ERRO--pthread_join\n");
					return 4;
				}
			}
			
			//destroi a variavel de exclusao mutua
			pthread_mutex_destroy(&lock);
			
			//tomada de tempo
			GET_TIME(fim);
			printf("Tempo de execucao: %lf\n", fim-ini);
			
			//libera as areas de memoria alocadas
			free(tid);
		}	
	}
	//exibir resultados
	printf("MDC = %lld\n", mdc);
			
	return 0;
}
