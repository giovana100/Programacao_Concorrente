/*********************************************************************
 *                                                                   *
 *             Giovana Pinho Garcia - 180101374                      *
 *                                                                   *
 *********************************************************************/ 
//Semaforos

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários

#define CARTAS 20 //quantidade de cartas na mochila

sem_t sem_mochila;   //semaforo para controlar a quantidade de cartas na mochila
sem_t sem_pombo;     //semaforo para controlar o acesso ao pombo
sem_t sem_usuarios;  //semaforo para controlar os usuarios

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;   //lock para auxiliar o acesso a mochila

int n_cartas = 0;

void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
    
    sem_init(&sem_mochila, 0, CARTAS);
    sem_init(&sem_pombo, 0, 0);
    sem_init(&sem_usuarios, 0, 0);
    
    int i;

    pthread_t usuario[N];
    int *id;
    for(i = 0; i < N; i++){
         id = (int *) malloc(sizeof(int));
          *id = i;
	pthread_create(&(usuario[i]),NULL,f_usuario,  (void *) (id));
    }
     pthread_t pombo;
     id = (int *) malloc(sizeof(int));
     *id = 0;
     pthread_create(&(pombo),NULL,f_pombo, (void*) (id));

     pthread_join(pombo,NULL);
   
}


void * f_pombo(void *arg){
  
    while(1){
        //Inicialmente está em A, aguardar/dorme a mochila ficar cheia (20 cartas)
        sem_wait(&sem_pombo);

        //Leva as cartas para B e volta para A
        printf("Pombo está levando as 20 cartas de A para B\n");
        sleep(3);
        
        //Acordar os usuários   
        for(int i=0; i<CARTAS; i++){
            //liberar todas as posicoes de acesso da mochila
            sem_post(&sem_mochila);
            n_cartas=0;
        }
        
        //acorda os usuarios
        sem_post(&sem_usuarios);

    }
}

void * f_usuario(void *arg){
    
    int id = *(int*) arg;
    
    while(1){
	    //Escreve uma carta
	    printf("O usuario %d esta escrevendo uma carta\n", id);
	    sleep(1);

	    pthread_mutex_lock(&lock);
	    
        //Caso o pombo não esteja em A ou a mochila estiver cheia, então dorme	
        if(sem_trywait(&sem_mochila) == 0){ //conseguiu colocar uma carta
            //Posta sua carta na mochila do pombo
            n_cartas ++;
            printf("O usuario %d esta colocando sua carta na mochila.  Qtd cartas = %d\n", id, n_cartas);
            sleep(1);
        }
        else{
            //Caso a mochila fique cheia, acorda o pombo
            sem_post(&sem_pombo);
            //trava os usuarios enquanto o pombo nao volta
            sem_wait(&sem_usuarios);
        }
        
        pthread_mutex_unlock(&lock);
    }
}
