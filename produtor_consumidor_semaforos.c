/*---------------------------------------------------------------------------------------------

Giovana Pinho Garcia - 18/0101374

---------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define PR 1 //número de produtores
#define CN 1 // número de consumidores
#define N 5  //tamanho do buffer


sem_t mutex;  //semaforo que controla o acesso a regiao critica
sem_t empty;  //semaforo que contabiliza a quantidade de lugares vazios no buffer
sem_t full;   //semaforo que contabiliza a quantidade de lugares cheios no buffer


int item = 0;  //inicializando a variavel item que contabiliza o buffer


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

  sem_init(&mutex, 1, 1);
  sem_init(&empty, N, N);
  sem_init(&full, 0, 0);
   
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

void * produtor (void* pi)
{
 
  while(1)
  {

    sleep(2);

    if (item < N){                //confere se a quantidade de itens no buffer eh maxima
      sem_wait(&empty);           //decrementa o valor de empty
      sem_wait(&mutex);           //entra na regiao critica

      item++;                     //aumenta a quantidade de itens no buffer

      printf("Produtores estão produzindo....    qtd item: %d\n", item);

      sem_post(&mutex);           //sai da regiao critica
      sem_post(&full);            //incrementa valor de full
    }

  }
  pthread_exit(0);
  
}

void * consumidor (void* pi)
{
 while(1)
  {

    sleep(5);

    if(item > 0){              //confere se o buffer esta vazio
      sem_wait(&full);         //decrementa valor de full
      sem_wait(&mutex);        //entra na regiao critica

      item--;                  //diminui a quantidade de itens no buffer

      printf("Consumidores estão consumindo....    qtd item: %d\n", item);

      sem_post(&mutex);        //sai da regiao critica
      sem_post(&empty);        //incrementa valor de empty
    }
 
  }
  pthread_exit(0);
  
}
