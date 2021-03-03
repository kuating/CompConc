#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

#include "timer.h"

#define M 10
#define N 600

//Numero de threads BuscaPadrao
int B = 3;

//Arquivo a ser lido
FILE *fp;

//Respostas das 3 buscas
char resposta1[100];
char resposta2[100];
char resposta3[100];

//VARIAVEIS COMPARTILHADAS

//Buffer
char buffer[M][N];

//Conta progresso das BuscaPadrao em cada bloco
//começa com 0. ao atingir B, libera bloco para escrita e zera.
char blocoProg[M];

int blocoBuffer = 0; //bloco que configBuffer esta escrevendo
int blocoBusca = 0; //ultimo bloco ainda sendo lido por uma das BuscaPadrao

int blocosParaLer = 0;

int complete = 0; //1 se ConfigBuffer terminou de ler arquivo, 0 senao

long long int nElem = 0; //numero de elementos no arquivo


//Variaveis de sincronizacao
pthread_cond_t cond_nextBlock; //bloco liberado para leitura
pthread_cond_t cond_lastBlock; //bloco liberado para escrita
pthread_mutex_t mutex;

//Busca de Padrao 1 - Maior sequencia de valores iguais
void * BuscaPadrao1 () {
	
	int blocoAtual = 0; //inicializo bloco a ler/em leitura
	int lastInBlock = N-1; //quando completar, nao ler ultimo bloco todo
	int count=0;
	
	//Variaveis especificas dessa busca de padrao
	int nMax = -1;
	int maxLenght = 0;
	int startPos = 0;
	int nTemp = -1;
	int tempLenght = 0;
	int tempPos = 0;
	
	//inicio da execução
	while(1){
		pthread_mutex_lock(&mutex);
		//Checa se pode usar bloco
		if(blocosParaLer==0 || blocoBuffer == blocoAtual)
			if(complete==0) pthread_cond_wait(&cond_nextBlock, &mutex);
		if(blocoBuffer == blocoAtual && complete==1){
			//Se buffer completo e esta ultimo bloco:
			//atualiza posicao de ultimo elemento do bloco
			lastInBlock = (nElem-1)%N;
		}
		pthread_mutex_unlock(&mutex);
		
		
		//Busca de Padrao
		for(int i=0; i<=lastInBlock; i++){
			count++;
			if(buffer[blocoAtual][i] != nTemp){
				if(tempLenght>maxLenght){
					nMax = nTemp; maxLenght = tempLenght; startPos=tempPos;
				}
				nTemp = buffer[blocoAtual][i];
				tempLenght = 1;
				tempPos = count;
			}
			else{
				tempLenght++;
			}
		}
		
		
		pthread_mutex_lock(&mutex);
		
		//se thread acabou
		if(blocoBuffer == blocoAtual && complete==1){
			pthread_mutex_unlock(&mutex);
			//preenche string resultado
			sprintf(resposta1,"Maior sequencia de valores identicos: %d %d %d\n",startPos,maxLenght,nMax);
			pthread_exit(NULL);
		}
		//Thread acabou com esse bloco
		blocoProg[blocoAtual]++;
		//Se todas as threads ja analisaram bloco, pode sobrescrever
		if(blocoProg[blocoAtual] == B){
			blocoProg[blocoAtual]=0;
			//novo bloco ja explorado por todas threads
			blocoBusca = (blocoAtual+1)%M;
			blocosParaLer--;
			pthread_cond_signal(&cond_lastBlock);
		}
		//busca proximo bloco
		blocoAtual=(blocoAtual+1)%M;
		
		
		pthread_mutex_unlock(&mutex);
	}
}

//Busca de Padrao 2 - Quantidade de triplas 
void * BuscaPadrao2 () {
	
	int blocoAtual = 0; //inicializo bloco a ler/em leitura
	int lastInBlock = N-1; //quando completar, nao ler ultimo bloco todo
	
	//Variaveis especificas dessa busca de padrao
	int nTriplas = 0;
	int nTemp = -1;
	int nSeq = 0;
	
	//inicio da execução
	while(1){
		pthread_mutex_lock(&mutex);
		//Checa se pode usar bloco
		if(blocosParaLer==0 || blocoBuffer == blocoAtual)
			if(complete==0) pthread_cond_wait(&cond_nextBlock, &mutex);
		if(blocoBuffer == blocoAtual && complete==1){
			//Se buffer completo e esta ultimo bloco:
			//atualiza posicao de ultimo elemento do bloco
			lastInBlock = (nElem-1)%N;
		}
		pthread_mutex_unlock(&mutex);
		
		
		//Busca de Padrao
		for(int i=0; i<=lastInBlock; i++){
			if(buffer[blocoAtual][i] != nTemp){
				nTemp = buffer[blocoAtual][i];
				nSeq=1;
			}
			else{
				nSeq++;
				if(nSeq == 3){
					nTriplas++;
					nSeq=0;
				}
			}
		}
		
		
		pthread_mutex_lock(&mutex);
		
		//se thread acabou
		if(blocoBuffer == blocoAtual && complete==1){
			pthread_mutex_unlock(&mutex);
			//preenche string resultado
			sprintf(resposta2,"Quantidade de triplas: %d\n",nTriplas);
			pthread_exit(NULL);
		}
		//Thread acabou com esse bloco
		blocoProg[blocoAtual]++;
		//Se todas as threads ja analisaram bloco, pode sobrescrever
		if(blocoProg[blocoAtual] == B){
			blocoProg[blocoAtual]=0;
			//novo bloco ja explorado por todas threads
			blocoBusca = (blocoAtual+1)%M;
			blocosParaLer--;
			pthread_cond_signal(&cond_lastBlock);
		}
		//busca proximo bloco
		blocoAtual=(blocoAtual+1)%M;
		
		
		pthread_mutex_unlock(&mutex);
	}
}

//Busca de Padrao 3 - Quantidade de <012345> 
void * BuscaPadrao3 () {
	
	int blocoAtual = 0; //inicializo bloco a ler/em leitura
	int lastInBlock = N-1; //quando completar, nao ler ultimo bloco todo
	
	//Variaveis especificas dessa busca de padrao
	int n012345 = 0;
	int nSeq = 0;
	int seq[6]={0,1,2,3,4,5};
	
	//inicio da execução
	while(1){
		pthread_mutex_lock(&mutex);
		//Checa se pode usar bloco
		if(blocosParaLer==0 || blocoBuffer == blocoAtual)
			if(complete==0) pthread_cond_wait(&cond_nextBlock, &mutex);
		//Se o buffer está completo, blocoBuffer eh o ultimo bloco.
		if(blocoBuffer == blocoAtual && complete==1){
			//Se buffer completo e esta ultimo bloco:
			//atualiza posicao de ultimo elemento do bloco
			lastInBlock = (nElem-1)%N;
		}
		pthread_mutex_unlock(&mutex);
		
		
		//Busca de Padrao
		for(int i=0; i<=lastInBlock; i++){
			if(buffer[blocoAtual][i] == seq[nSeq]){
				nSeq++;
				if(nSeq == 6){
					n012345++;
					nSeq = 0;
				}
			}else nSeq = 0;
		}
		
		
		pthread_mutex_lock(&mutex);
		
		//se thread acabou
		if(blocoBuffer == blocoAtual && complete==1){
			pthread_mutex_unlock(&mutex);
			//preenche string resultado
			sprintf(resposta3,"Quantidade de ocorrencias da sequencia <012345>: %d\n",n012345);
			pthread_exit(NULL);
		}
		//Thread acabou com esse bloco
		blocoProg[blocoAtual]++;
		//Se todas as threads ja analisaram bloco, pode sobrescrever
		if(blocoProg[blocoAtual] == B){
			blocoProg[blocoAtual]=0;
			//novo bloco ja explorado por todas threads
			blocoBusca = (blocoAtual+1)%M;
			blocosParaLer--;
			pthread_cond_signal(&cond_lastBlock);
		}
		//busca proximo bloco
		blocoAtual=(blocoAtual+1)%M;
		
		
		pthread_mutex_unlock(&mutex);
	}
}

//Configura Buffer
void * ConfigBuffer () {
	
	long long int counter = 0;
	for(int i=0;i<M;i++) blocoProg[i] = 0;
	int sub = 0;
	while(1){
		sub=blocoBuffer;
		//transfere os proximos N inteiros para o buffer
		//PS: se acabar arquivo, parar.
		for(long long int i = 0; i<N; i++){
			if(!fread(&buffer[sub][i] , sizeof(int) , 1 , fp ))
				break;
			counter++;
		}
	
		pthread_mutex_lock(&mutex);
		//Se acabaram os elementos
		if(counter == nElem){
			complete=1;
			pthread_cond_broadcast(&cond_nextBlock);
			pthread_mutex_unlock(&mutex);
			pthread_exit(NULL);
		}
		//Pega novo bloco para preencher
		blocoBuffer=(blocoBuffer+1)%M;
		blocosParaLer++;
		//libera bloco para leitura
		pthread_cond_broadcast(&cond_nextBlock);
		//Se novo bloco ainda esta sendo usado
		if(blocosParaLer==N){
			pthread_cond_wait(&cond_lastBlock, &mutex);
		}
		pthread_mutex_unlock(&mutex);
	}
}


int main(int argc, char** argv){
	
	double ini,fim; //Tomada de tempo
	
	pthread_t buscaPadrao[B];
	pthread_t configBuffer;
	
	//Abrir arquivo
	if( argc > 1 )
		fp = fopen( argv[1] , "rb" );
	else{
		printf("Insira %s <nome do arquivo a ler>\n",argv[0]);
		return 0;
	}
	if( fp == NULL ){
		printf("%s nao encontrado\n",argv[1]);
		return 0;
	}
	
	/* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init (&cond_nextBlock, NULL);
	pthread_cond_init (&cond_lastBlock, NULL);
	
	//Inicializar nElem com primeiro numero (long long) do arquivo
	fread(&nElem,sizeof(long long int),1,fp);
	
	//Tomada de tempo
	GET_TIME(ini);
	
	//Inicia as 4 threads
	pthread_create(&buscaPadrao[0], NULL, BuscaPadrao1, NULL);
	pthread_create(&buscaPadrao[1], NULL, BuscaPadrao2, NULL);
	pthread_create(&buscaPadrao[2], NULL, BuscaPadrao3, NULL);
	pthread_create(&configBuffer, NULL, ConfigBuffer, NULL);
	
	//Join nas 4 threads
	pthread_join(configBuffer, NULL);
	for (int i = 0; i < B; i++) {
		pthread_join(buscaPadrao[i], NULL);
	}
	
	//Tomada de tempo
	GET_TIME(fim);
	printf("Tempo de execucao: %lf\n", fim-ini);
	
	//Printar respostas
	printf("%s\n",resposta1);
	printf("%s\n",resposta2);
	printf("%s\n",resposta3);
	
	//Limpar memoria alocada
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond_nextBlock);
	pthread_cond_destroy(&cond_lastBlock);
	
	fclose(fp);
	return(0);
}
