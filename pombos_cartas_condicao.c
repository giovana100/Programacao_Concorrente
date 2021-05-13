/*********************************************************************
 *                                                                   *
 *             Giovana Pinho Garcia - 180101374                      *
 *                                                                   *
 *********************************************************************/ 
//Variaveis de Condicao e Locks

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários

#define CARTAS 20 //quantidade de cartas na mochila

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; //lock para utilizar com a variavel de condicao

pthread_cond_t cond_usuarios = PTHREAD_COND_INITIALIZER;   //variavel de condicao utilizada nos usuarios
pthread_cond_t cond_pombo = PTHREAD_COND_INITIALIZER;      //variavel de condicao utilizada no pombo

int mochila = 0;

void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
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
        pthread_mutex_lock(&lock);
            //se a mochila nao estiver cheia o pombo esta dormindo
            while(mochila < CARTAS){
                pthread_cond_wait(&cond_pombo, &lock);
            }
        pthread_mutex_unlock(&lock);
        
        //Leva as cartas para B e volta para A
        printf("Pombo está levando as 20 cartas de A para B\n");
        sleep(3);
        pthread_mutex_lock(&lock);
            //esvazia a mochila
            mochila = 0;
            //Acordar os usuários   
            pthread_cond_broadcast(&cond_usuarios);
        pthread_mutex_unlock(&lock);
    }
}

void * f_usuario(void *arg){
    
    while(1){
	    //Escreve uma carta
	    printf("Um usuario esta escrevendo uma carta\n");
	    sleep(1);
        //Caso o pombo não esteja em A ou a mochila estiver cheia, então dorme	
        pthread_mutex_lock(&lock);
            //se a mochila estiver cheia os usuarios estao dormindo
            while(mochila == CARTAS){
                pthread_cond_wait(&cond_usuarios, &lock);
            }
            mochila++;
        
        //Posta sua carta na mochila do pombo
        printf("Usuario esta colocando sua carta na mochila. Qtd cartas = %d\n", mochila);
        sleep(1);

            //Caso a mochila fique cheia, acorda o pombo
            if(mochila == CARTAS){
                pthread_cond_signal(&cond_pombo);
            }
        pthread_mutex_unlock(&lock);
    }
}
