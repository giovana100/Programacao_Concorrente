/*---------------------------------------------------------------------------------------------

Giovana Pinho Garcia - 18/0101374

---------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAXSIZE 10000  /* maximum matrix size */

pthread_barrier_t barrier;  
int size;


int matrix1[MAXSIZE][MAXSIZE];
int matrixR[MAXSIZE];

void *Worker(void *);

int main(int argc, char *argv[]) {
  int i, j;
 
  
  size = atoi(argv[1]);
  
   if(size > MAXSIZE){
	printf("Tamanho muito grande!\n");
        return 0;	
   }

  pthread_t workerid[size];
  

  pthread_barrier_init(&barrier, NULL, size);

  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++)
      matrix1[i][j] = 1;


  for (i = 0; i < size; i++)
      matrixR[i] = 0;


  int * id;
  for (i = 0; i < size; i++){
     id = (int *) malloc(sizeof(int));
     *id = i;
     pthread_create(&workerid[i], NULL, Worker, (void *) (id));

  }

  for (i = 0; i < size; i++){
       if(pthread_join(workerid[i], NULL)){
          printf("\n ERROR joining thread");
          exit(1);
       }
  }
  printf("Bye!\n");
}


void *Worker(void *arg) {
  int myid = *(int *)(arg);
  int j, k;
  int soma = 0;

  int self = pthread_self();
 
  printf("worker %d (pthread id %d) has started\n", myid, self);


  //fazer a soma de cada linha da matriz
	for(k = 0; k < size; k++){
    matrixR[myid]=matrixR[myid]+matrix1[myid][k];
	}


  pthread_barrier_wait(&barrier);

  if (myid == 0) {
    printf("\n");
    
    //somar as somas das linhas
    for(k = 0; k < size; k++){
     soma += matrixR[k];
    }

    //printar a soma das linhas
    printf("soma = %d\n\n", soma);
  }


}
