/*---------------------------------------------------------------------------------------------

Giovana Pinho Garcia - 18/0101374

---------------------------------------------------------------------------------------------*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 //macacos que andam de A para B
#define MB 10 //macacos que andam de B para A

pthread_mutex_t lock_corda = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_AB = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_BA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_BAquer = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_ABquer = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_gorila = PTHREAD_MUTEX_INITIALIZER;



int num_AB=0;
int num_BA=0;
int num_gorila=0;

void * macacoAB(void * a) {
    int i = *((int *) a);    
    while(1){
        //Procedimentos para acessar a corda
        if(num_gorila > 0){
          //trava caso o gorila queira passar
			    pthread_mutex_lock(&lock_gorila);
        }

        if(num_BA > 0){
          //trava caso o BA queira passar
			    pthread_mutex_lock(&lock_BAquer);
        }
        //trava para incrementar o contador de AB
			  pthread_mutex_lock(&lock_AB);
          num_AB++;
          if(num_AB == 1){
            //trava para macacos AB passar pela corda
			      pthread_mutex_lock(&lock_corda);
          }
			  pthread_mutex_unlock(&lock_AB);

	      printf("Macaco %d passado de A para B \n",i);
	      sleep(10);

        //trava para decrementar o contador de AB
			  pthread_mutex_lock(&lock_AB);
          num_AB--;
          //destrava o lock se nao tiver mais macacos AB
          if(num_AB == 0){
            //destrava corda
			      pthread_mutex_unlock(&lock_corda);
          }
			  pthread_mutex_unlock(&lock_AB);
        //destrava ABquer pq nao tem mais macacos AB
			  pthread_mutex_unlock(&lock_ABquer);

        
	      //Procedimentos para quando sair da corda
    }
    pthread_exit(0);
}

void * macacoBA(void * a) {
    int i = *((int *) a);    
    while(1){
        //Procedimentos para acessar a corda
        if(num_gorila > 0){
          //trava caso o gorila queira passar
			    pthread_mutex_lock(&lock_gorila);
        }

        if(num_AB > 0){
          //trava caso o AB queira passar
			    pthread_mutex_lock(&lock_ABquer);
        }
        //trava para incrementar o contador de BA
			  pthread_mutex_lock(&lock_BA);
          num_BA++;
          if(num_BA == 1){
            //trava para macacos BA passar pela corda
			      pthread_mutex_lock(&lock_corda);
          }
			  pthread_mutex_unlock(&lock_BA);

	      printf("Macaco %d passado de B para A \n",i);
	      sleep(10);
			  
        //trava para decrementar o contador de BA
        pthread_mutex_lock(&lock_BA);
          num_BA--;
          if(num_BA == 0){
            //destrava corda
			      pthread_mutex_unlock(&lock_corda);
          }
			  pthread_mutex_unlock(&lock_BA);
        //destrava BAquer pq nao tem mais macacos BA
			  pthread_mutex_unlock(&lock_BAquer);


	      //Procedimentos para quando sair da corda
    }
    pthread_exit(0);
}



void * gorila(void * a){
    while(1){
	    //Procedimentos para acessar a corda

      num_gorila++;

			pthread_mutex_lock(&lock_corda);

	    printf("Gorila passado de A para B \n");
	    sleep(5);

			pthread_mutex_unlock(&lock_corda);

      num_gorila--;

			pthread_mutex_unlock(&lock_gorila);

      //Procedimentos para quando sair da corda
    }
    pthread_exit(0);
}

int main(int argc, char * argv[])
{
    pthread_t macacos[MA+MB];
    int *id;
    int i = 0;

    for(i = 0; i < MA+MB; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        if(i%2 == 0){
          if(pthread_create(&macacos[i], NULL, &macacoAB, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        }else{
          if(pthread_create(&macacos[i], NULL, &macacoBA, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        }
    }
    pthread_t g;
    pthread_create(&g, NULL, &gorila, NULL);

  
    pthread_join(macacos[0], NULL);
    return 0;
}
