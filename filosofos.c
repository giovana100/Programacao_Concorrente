/*********************************************************************
 *                                                                   *
 *             Giovana Pinho Garcia - 180101374                      *
 *                                                                   *
 *********************************************************************/ 

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define N 5
#define ESQ(id) (id)
#define DIR(id)	(id+1)%N


int state[N];
sem_t s[N];


//inicializando as variaveis que serao colocadas nos estados dos filosofos
int thinking = 0;
int hungry = 1;
int eating = 2;

void *filosofos (void *arg);

void pega_talher (int n);
void devolve_talher (int n);

int main () {
	int i;
	int *id;
	//semaforo 
	for (i=0; i<N; i++) {
		sem_init(&s[i], 0, 1);
	}
	pthread_t r[N]; 

	//criacao das threads de filosofos
       for (i = 0; i < N ; i++) {
	     id = (int *) malloc(sizeof(int));
	     *id = i;
	     pthread_create(&r[i], NULL, filosofos, (void*) (id));
	    }

        pthread_join(r[0],NULL);
	return 0;
}


void *filosofos (void *arg) {
	int n = *((int *) arg);
	while(1) {
		//pensar
		printf("Filosofo %d pensando ...\n", n);
		sleep(3);
		pega_talher(n);
		//comer
		printf("\tFilosofo %d comendo ...\n", n);
		sleep(3);
        printf("\tFilosofo %d acabou de comer ...\n", n);
		devolve_talher(n);
	} 
}

void pega_talher (int n) {
    //flag que vai ser usada como condicao do while
    int conseguiu=0;
    //while que vai parar quando o filosofo conseguir comer
    while(!conseguiu){
        //muda o estado do filosofo para hugry
        state[n] = hungry;
        
        //confere se algum vizinho do filosofo esta comendo
        if(state[ESQ(n)] != eating && state[DIR(n)] != eating){
            //coloca o filosofo como eating para garantir que ninguem pegara os talheres dele
            state[n] = eating;
        }
        
        //confere se o filosofo conseguiu comecar a comer
        if(state[n] == eating){
            //pega seus talheres
        	sem_wait(&s[ESQ(n)]);
        	sleep(1);
        	sem_wait(&s[DIR(n)]);
        	conseguiu = 1;
        }
    }
}

void devolve_talher (int n) {
    //devolve os talheres
    sem_post(&s[ESQ(n)]);
    sem_post(&s[DIR(n)]);
    //muda o estado do filosofo para thinking
    state[n] = thinking;
}


