/* Threads 1 e 2 incrementam x em 1, que inicia como 0
 * Quando x for 2, as outras 2 threads serão executadas
 * por broadcast
 * 
 * 1 “tudo bem?”
 * 2 “bom dia!”
 * ------------------
 * 3 “ate mais!”
 * 4 “boa tarde!”
 */

#include <pthread.h>
#include <stdio.h>

#define NTHREADS  4

/* Variaveis globais */
int x = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

void *T1 (){
	
	printf("Thread 1: tudo bem?\n");
	pthread_mutex_lock(&x_mutex);
	x++;
	if(x==2){ pthread_cond_broadcast(&x_cond); printf("---\n"); }
	pthread_mutex_unlock(&x_mutex);
	pthread_exit(NULL);
}

void *T2 (){
	
	printf("Thread 2: bom dia!\n");
	pthread_mutex_lock(&x_mutex);
	x++;
	if(x==2){ pthread_cond_broadcast(&x_cond); printf("---\n"); }
	pthread_mutex_unlock(&x_mutex);
	pthread_exit(NULL);
}

void *T3 (){
	
	pthread_mutex_lock(&x_mutex);
	if(x<2){
		pthread_cond_wait(&x_cond, &x_mutex);
	}
	printf("Thread 3: ate mais!\n");
	pthread_mutex_unlock(&x_mutex); 
	pthread_exit(NULL);
}

void *T4 (){
	
	pthread_mutex_lock(&x_mutex);
	if(x<2){
		pthread_cond_wait(&x_cond, &x_mutex);
	}
	printf("Thread 4: boa tarde!\n");
	pthread_mutex_unlock(&x_mutex); 
	pthread_exit(NULL);
}

/* Funcao principal */
int main(int argc, char *argv[]) {
	
	int i; 
	pthread_t threads[NTHREADS];

	/* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
	pthread_mutex_init(&x_mutex, NULL);
	pthread_cond_init (&x_cond, NULL);

	/* Cria as threads */
	pthread_create(&threads[0], NULL, T1, NULL);
	pthread_create(&threads[1], NULL, T2, NULL);
	pthread_create(&threads[2], NULL, T3, NULL);
	pthread_create(&threads[3], NULL, T4, NULL);
	
	/* Espera todas as threads completarem */
	for (i = 0; i < NTHREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	
	/* Desaloca variaveis e termina */
	pthread_mutex_destroy(&x_mutex);
	pthread_cond_destroy(&x_cond);
	return 0;
}
