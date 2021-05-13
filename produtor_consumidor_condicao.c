/*---------------------------------------------------------------------------------------------

Giovana Pinho Garcia - 18/0101374

---------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PR 1 //número de produtores
#define CN 1 // número de consumidores
#define N 5  //tamanho do buffer

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t n_produtor = PTHREAD_COND_INITIALIZER;
pthread_cond_t n_consumidor = PTHREAD_COND_INITIALIZER;

int prodQuer = 0; //numero de produtores que estao prontos para produzir
int consQuer = 0; //numero de coonsumidores que estao prontos para consumir
int cons = 0; //consumidores acessando a regiao crítica
int prod = 0; //produtores acessando a regiao critica

int produto = 0; //produtos prontos


void * produtor(void * meuid);
void * consumidor (void * meuid);

void main(argc, argv)
int argc;
char *argv[];
{

  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[PR];
   
  for (i = 0; i < PR; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, produtor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t tCid[CN];

  for (i = 0; i < CN; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tCid[i], NULL, consumidor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }
 
  pthread_join(tid[0],NULL);

}

void * produtor (void* pi){
 
  while(1){
	sleep(2);

   //lock para usar com variaveis condicao
	 pthread_mutex_lock(&lock);

    //loop que faz o produtor dormir se algum consumidor estiver consumindo ou ja tiver a quantidade maxima de produtos
    while((cons > 0) || produto >= 5){
	      pthread_cond_wait(&n_produtor, &lock);
    }

    prod++;

	 pthread_mutex_unlock(&lock);

	printf("Produtor está produzindo.    num prod = %d \n", produto+1);
  sleep(1);
  //incrementa produto que foi produzido
  produto++; 

	 pthread_mutex_lock(&lock);

    prod--;

    //acorda os produtores e consumidores
    pthread_cond_signal(&n_produtor);
    pthread_cond_signal(&n_consumidor);

	 pthread_mutex_unlock(&lock);

  }
  pthread_exit(0);
  
}

void * consumidor (void* pi){
 while(1){

	 sleep(5);

   pthread_mutex_lock(&lock);

    //loop que faz o consumidor dormir se algum produtor estiver produzindo ou nao tiver produtos para consumir
    while(prod > 0 || produto == 0){
	      pthread_cond_wait(&n_consumidor, &lock);
    }

    cons++;

	 pthread_mutex_unlock(&lock);

	printf("Consumidor esta consumindo.   num prod = %d \n", produto-1);
  sleep(1);
  //decrementa produto que foi consumido
  produto--;

	 pthread_mutex_lock(&lock);

    cons--;

    //acorda os produtores e consumidores
    pthread_cond_signal(&n_produtor);
    pthread_cond_signal(&n_consumidor);

	 pthread_mutex_unlock(&lock);
 
  }
  pthread_exit(0);
  
}
