#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h> //para usar floor()

int* vetor;

//Limitantes de em quais porcoes do vetor cada thread vai trabalhar.
typedef struct{
	int firstIndex;
	int lastIndex;
} t_Args;

//Thread que incrementa uma porcao do vetor limitada por args
void * IncrementaVetor (void * arg){
	t_Args * args = (t_Args *) arg;
	for(int j = args->firstIndex; j < args->lastIndex; j++){
		vetor[j] += 1;
	}
	free(arg);
	pthread_exit(NULL);
}

int main(void){
	int nThreads = 2; // Numero de threads alem da principal
	
	int tam = 100; // Tamanho do vetor ( 10 < tam < 100 )
	
	//Inserir numero de threads
	printf("Insira o numero de threads (Enunciado pede 2) \n");
	scanf("%d",&nThreads);
	printf("\n");
	
	//Inserir tamanho do vetor a preencher
	printf("Insira tamanho N do vetor a preencher ( 10 < N < 100 ) \n");
	scanf("%d",&tam);
	printf("\n");
	
	//Identificadores das threads no sistema
	pthread_t * tid_sistema = (pthread_t *)malloc(sizeof(pthread_t) * nThreads);
	
	//Alocando memoria pro vetor
	vetor = (int *)malloc(sizeof(int) * tam);
	
	//Preenchendo o vetor
	for(int i = 0;i<tam;i++){
		vetor[i] = 0; 
	}
	
	//Imprimindo valores iniciais do vetor
	printf("Valores iniciais:\n");
	for(int i = 0;i<tam;i++){
		printf("%d ",vetor[i]); 
	}
	printf("\n");
	
	t_Args *arg;
	for(int i=0;i<nThreads;i++){
		arg = malloc(sizeof(t_Args));
		if(arg == NULL) {
			printf("--ERRO: malloc()\n"); exit(-1);
		}
		//Escolhendo porcoes para as threads trabalharem (1 por vez)
		arg->firstIndex = floor((double)tam/nThreads * i); 
		arg->lastIndex = floor((double)tam/nThreads * (i+1));
		//Iniciando as threads (1 por vez)
		if (pthread_create(&tid_sistema[i], NULL, IncrementaVetor, (void*) arg)) {
			printf("--ERRO: pthread_create()\n"); exit(-1);
		} 
	}
	
	//Esperando todas as threads nao principais acabarem
	for(int i=0; i<nThreads; i++){
		if(pthread_join(tid_sistema[i], NULL)) {
			printf("--ERRO: pthread_join() \n"); exit(-1); 
		} 
	}
	
	//Imprimindo valores finais do vetor
	printf("\n");
	printf("Valores finais:\n");
	for(int i = 0;i<tam;i++){
		printf("%d ",vetor[i]); 
	}
	printf("\n");
	
	//Não preciso do pthread_exit por causa do join
	return(0);
}
