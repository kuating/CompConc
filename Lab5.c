/* Rafael de Aguiar Ferreira - 117196643
 * 
 * Este programa gera os resultados previstos para o input:
 * ./Lab5 <solucao> <nthreads leitoras> <nthreads escritoras>
 * 
 * Sendo <solucao>:
 * 1-Sem Prioridades
 * 2-Prioridade para Escritores
 * 3-Prioridades Iguais
 * 
 * Para a 1: A solução apresentada é igual ao apresentado na aula4
 *
 * Para a 2: Mesma que a 1, com uma "barreira" no inicio das leitoras,
 * 			 se a fila de escritoras tiver alguma thread.
 * 
 * Para a 3: Mesma que a 2, mas quando existe uma escritora na fila E
 * 			 as leituras acabaram, a ultima thread leitora manda um
 * 			 broadcast para as leitoras, alem das escritoras.
 * 			 Ainda, a leitora pode entrar se tiver escritora na fila
 * 			 mas nao existem leitoras executando mais.
 * 			 (Essencialmente, quando uma escritora quiser entrar, as
 * 			 leitoras vao bloquear, para poder fazer uma decisao
 * 			 com nenhuma thread executando. Se entrar uma escritora,
 * 			 bloqueia as leitoras. Se entrar o grupo de leitoras em
 * 			 espera, bloqueia as escritoras.)
 * 
 * OBS1 Destaquei no terminal os bloqueios, para facilitar a
 * 		visualizacao das partes relevantes.
 * 
 * OBS2 Fiz diferentes InicLeit, InicEscr, FimLeit, FimEscr para cada
 * 		solucao. As threads da escritora e leitora sao iguais para
 * 		todas, fazendo as substituicoes necessarias com IFs.
 */

#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define N 5 //numero de posicoes no vetor data

int L = 4; //numero de threads leitoras
int E = 2; //numero de threads escritoras

int data[N]; //vetor de dados compartilhado

//variaveis do problema
int leit=0; //contador de threads lendo
int escr=0; //contador de threads escrevendo

//tipo de solucao do enunciado (1,2 ou 3)
int sol;

//barreira para solucao 2, tamanho da fila de escritores
int efila=0;

//barreira para solucao 3, quantas threads das L+E ja finalizaram
//no ciclo. Queda de desempenho mas sem risco de inanicao
int bar3=0;
pthread_cond_t cond_3;

//variaveis para sincronizacao
pthread_mutex_t mutex;
pthread_cond_t cond_leit, cond_escr;


////////////////////SOLUCAO1////////////////////////
//entrada leitura
void InicLeit (int id) {
	pthread_mutex_lock(&mutex);
	printf("L[%d] quer ler\n\n", id);
	while(escr > 0) {
		printf("L[%d] bloqueou------------------------bloqueio\n\n", id);
		pthread_cond_wait(&cond_leit, &mutex);
		printf("L[%d] desbloqueou------------------------bloqueio\n\n", id);
	}
	leit++;
	pthread_mutex_unlock(&mutex);
}

//saida leitura
void FimLeit (int id) {
	pthread_mutex_lock(&mutex);
	printf("L[%d] terminou de ler\n\n", id);
	leit--;
	if(leit==0) pthread_cond_signal(&cond_escr);
	pthread_mutex_unlock(&mutex);
}

//entrada escrita
void InicEscr (int id) {
	pthread_mutex_lock(&mutex);
	printf("E[%d] quer escrever\n\n", id);
	while((leit>0) || (escr>0)) {
		printf("E[%d] bloqueou------------------------bloqueio\n\n", id);
		pthread_cond_wait(&cond_escr, &mutex);
		printf("E[%d] desbloqueou------------------------bloqueio\n\n", id);
	}
	escr++;
	pthread_mutex_unlock(&mutex);
}

//saida escrita
void FimEscr (int id) {
	pthread_mutex_lock(&mutex);
	printf("E[%d] terminou de escrever\n\n", id);
	escr--;
	pthread_cond_signal(&cond_escr);
	pthread_cond_broadcast(&cond_leit);
	pthread_mutex_unlock(&mutex);
}

////////////////////SOLUCAO1////////////////////////


////////////////////SOLUCAO2////////////////////////
//entrada leitura
void InicLeit2 (int id) {
	pthread_mutex_lock(&mutex);
	printf("L[%d] quer ler\n\n", id);
	while(escr > 0 || efila>0) {
		printf("L[%d] bloqueou------------------------bloqueio\n\n", id);
		pthread_cond_wait(&cond_leit, &mutex);
		printf("L[%d] desbloqueou------------------------bloqueio\n\n", id);
	}
	leit++;
	pthread_mutex_unlock(&mutex);
}

//saida leitura
void FimLeit2 (int id) {
	pthread_mutex_lock(&mutex);
	printf("L[%d] terminou de ler\n\n", id);
	leit--;
	if(leit==0) pthread_cond_signal(&cond_escr);
	pthread_mutex_unlock(&mutex);
}

//entrada escrita
void InicEscr2 (int id) {
	pthread_mutex_lock(&mutex);
	efila++;//indica tamanho da fila de escritores
	printf("E[%d] quer escrever\n\n", id);
	while((leit>0) || (escr>0)) {
		printf("E[%d] bloqueou------------------------bloqueio\n\n", id);
		pthread_cond_wait(&cond_escr, &mutex);
		printf("E[%d] desbloqueou------------------------bloqueio\n\n", id);
	}
	escr++;
	pthread_mutex_unlock(&mutex);
}

//saida escrita
void FimEscr2 (int id) {
	pthread_mutex_lock(&mutex);
	printf("E[%d] terminou de escrever\n\n", id);
	efila--;
	escr--;
	pthread_cond_signal(&cond_escr);
	pthread_cond_broadcast(&cond_leit);
	pthread_mutex_unlock(&mutex);
}
////////////////////SOLUCAO2////////////////////////


////////////////////SOLUCAO3////////////////////////
//entrada leitura
void InicLeit3 (int id) {
	pthread_mutex_lock(&mutex);
	printf("L[%d] quer ler\n\n", id);
	while(escr > 0 || (efila>0 && leit>0)) {
		printf("L[%d] bloqueou------------------------bloqueio\n\n", id);
		pthread_cond_wait(&cond_leit, &mutex);
		printf("L[%d] desbloqueou------------------------bloqueio\n\n", id);
	}
	leit++;
	pthread_mutex_unlock(&mutex);
}

//saida leitura
void FimLeit3 (int id) {
	pthread_mutex_lock(&mutex);
	printf("L[%d] terminou de ler\n\n", id);
	leit--;
	if(leit==0){
		pthread_cond_signal(&cond_escr);
		pthread_cond_broadcast(&cond_leit);
	}
	pthread_mutex_unlock(&mutex);
}

//entrada escrita
void InicEscr3 (int id) {
	pthread_mutex_lock(&mutex);
	efila++;//indica tamanho da fila de escritores
	printf("E[%d] quer escrever\n\n", id);
	while((leit>0) || (escr>0)) {
		printf("E[%d] bloqueou------------------------bloqueio\n\n", id);
		pthread_cond_wait(&cond_escr, &mutex);
		printf("E[%d] desbloqueou------------------------bloqueio\n\n", id);
	}
	escr++;
	pthread_mutex_unlock(&mutex);
}

//saida escrita
void FimEscr3 (int id) {
	pthread_mutex_lock(&mutex);
	printf("E[%d] terminou de escrever\n\n", id);
	efila--;
	escr--;
	pthread_cond_signal(&cond_escr);
	pthread_cond_broadcast(&cond_leit);
	pthread_mutex_unlock(&mutex);
}
////////////////////SOLUCAO3////////////////////////


//thread leitora
void * leitor (void * arg) {
	int *id = (int *) arg;
	//char str;
	while(1) {
		
		if(sol==1) InicLeit(*id);
		if(sol==2) InicLeit2(*id);
		if(sol==3) InicLeit3(*id);
		
		int soma = 0;  //soma dos elementos (usado para achar media)
		char str[N*2+2] = ""; //para for nao causar prints quebrados
		for(int i=0;i<N;i++){
			sprintf(str,"%s%d ",str,data[i]);
			soma+=data[i];
		}
		printf("Leitora %d diz:\nVetor: %s\nMedia = %.2lf\n\n",*id,str,(double)soma/N);
		if(sol==1) FimLeit(*id);
		if(sol==2) FimLeit2(*id);
		if(sol==3) FimLeit3(*id);
		sleep(1);
	} 
	free(arg);
	pthread_exit(NULL);
}

//thread escritora
void * escritor (void * arg) {
	int *id = (int *) arg;
	while(1) {
		if(sol==1) InicEscr(*id);
		if(sol==2) InicEscr2(*id);
		if(sol==3) InicEscr3(*id);
		printf("Escritora %d escrevendo\n\n", *id);
		for(int i=0;i<N;i++){
			if(i==0 || i==N-1) data[i] = *id;
			else data[i] = *id*2;
		}
		if(sol==1) FimEscr(*id);
		if(sol==2) FimEscr2(*id);
		if(sol==3) FimEscr3(*id);
		sleep(1);
	} 
	free(arg);
	pthread_exit(NULL);
}

//funcao principal
int main(int argc, char *argv[]) {
	//recebe e valida os parametros de entrada (dimensao do vetor)
	if(argc < 4){
		fprintf(stderr, "Digite: %s <solucao (1,2 ou 3)> <nthreads leitoras> <nthreads escritoras>\n", argv[0]);
		return 1;
	}
	sol = atoi(argv[1]);
	L = atoi(argv[2]);
	E = atoi(argv[3]);
	
	//identificadores das threads
	pthread_t tid[L+E];
	int id[L+E];

	//inicializa as variaveis de sincronizacao
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond_leit, NULL);
	pthread_cond_init(&cond_escr, NULL);
	pthread_cond_init(&cond_3, NULL);
	
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
