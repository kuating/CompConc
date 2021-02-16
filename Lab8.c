#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define L 3
#define E 1

#define N 5


sem_t em_e, em_l, escr, leit; //semaforos
int e=0, l=0; //globais
int data[N]; //vetor de dados compartilhado

void * leitor (void * arg) {
	int *id = (int *) arg;
	while(1){
		printf("Leitora %d quer ler\n",*id);
		sem_wait(&leit);
		sem_wait(&em_l); l++;
		if(l==1) sem_wait(&escr);
		sem_post(&em_l);
		sem_post(&leit);
		
		//le...
		printf("Leitora %d lendo\n",*id);
		int soma = 0;  //soma dos elementos (usado para achar media)
		char str[N*2+2] = ""; //para for nao causar prints quebrados
		for(int i=0;i<N;i++){
			sprintf(str,"%s%d ",str,data[i]);
			soma+=data[i];
		}
		sleep(1);
		//printf("Leitora %d diz:\nVetor: %s\nMedia = %.2lf\n\n",*id,str,(double)soma/N);
		//leu
		
		sem_wait(&em_l); l--;
		if(l==0) sem_post(&escr);
		sem_post(&em_l);
		
		sleep(1);
	}
	free(arg);
	pthread_exit(NULL);
}

void * escritor (void * arg) {
	int *id = (int *) arg;
	while(1){
		printf("Escritora %d quer escrever\n",*id);
		sem_wait(&em_e); e++;
		if(e==1) sem_wait(&leit);
		sem_post(&em_e);
		sem_post(&escr);
		
		//escreve...
		printf("Escritora %d escrevendo\n", *id);
		for(int i=0;i<N;i++){
			if(i==0 || i==N-1) data[i] = *id;
			else data[i] = *id*2;
		}
		sleep(1);
		//escreveu
		
		sem_post(&escr);
		sem_post(&em_e); e--;
		if(e==0) sem_post(&leit);
		sem_post(&em_e);
		
		sleep(1);
	}
	free(arg);
	pthread_exit(NULL);
}

//funcao principal
int main(int argc, char *argv[]) {
	//recebe e valida os parametros de entrada (dimensao do vetor)
	/*if(argc < 4){
		fprintf(stderr, "Digite: %s <solucao (1,2 ou 3)> <nthreads leitoras> <nthreads escritoras>\n", argv[0]);
		return 1;
	}
	sol = atoi(argv[1]);
	L = atoi(argv[2]);
	E = atoi(argv[3]);
	*/
	
	//identificadores das threads
	pthread_t tid[L+E];
	int id[L+E];
	
	//inicia os semaforos
	sem_init(&em_e, 0, 1);
	sem_init(&em_l, 0, 1);
	sem_init(&escr, 0, 1);
	sem_init(&leit, 0, 1);
	
	//Inicializa o vetor
	for(int i=0;i<N;i++){
		data[i]=0;
	}
	
	//cria as threads leitoras
	for(int i=0; i<L; i++) {
		id[i] = i+1;
		if(pthread_create(&tid[i], NULL, leitor, (void *) &id[i])) exit(-1);
	} 
	  
	//cria as threads escritoras
	for(int i=0; i<E; i++) {
		id[i+L] = i+1;
		if(pthread_create(&tid[i+L], NULL, escritor, (void *) &id[i+L])) exit(-1);
	} 

	pthread_exit(NULL);
	return 0;
	}
