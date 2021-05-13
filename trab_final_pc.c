/*********************************************************************
 *                                                                   *
 *             Giovana Pinho Garcia - 180101374                      *
 *                                                                   *
 *********************************************************************/ 

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CLARO    "\x1b[2m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[95m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

#define N_CLIENTES 50
#define N_MAX_CLIENTES 10
#define N_ITENS 12
#define N_CAIXAS 4

sem_t vagas_mercado;  //semaforo que controla as pessoas que entram no mercado
sem_t caixas;         //semaforo que controla o acesso aos caixas
sem_t caixas_livres;  //semaforo que controla a quantidade de caixas livres
sem_t pagamento;      //semaforo que controla o tempo de pagamento
sem_t repor;          //semaforo que controla quando os itens vao ser repostos
sem_t tempo_repor;    //semaforo que controla o tempo de reposicao

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int itens = N_ITENS;

void * f_caixas(void *v){
    while(1){
        int id = *(int*) v;

        //caixa espera algum cliente
        sem_wait(&caixas);

        //caixa atende o clente e demora x tempo
        sleep(1);
        printf(BLUE "Caixa %d atendeu um cliente\n"RESET, id); 

        //depois de atender um cliente
        sem_post(&pagamento);
    }
}

void* f_cliente(void* v){
    while(1){
        int id = *(int*) v;

        sleep(rand()%6+2);
        if(sem_trywait(&vagas_mercado) == 0){ //conseguiu entrar no mercado
            printf(GREEN "Cliente %d entrou no mercado \n" RESET,id);

            sleep(rand()%2); //cliente procurando o item
            //cliente pega um item
            pthread_mutex_lock(&lock);
                //se nao tiver mais itens, cliente acorda o repositor e espera o estoque ser colocado
                if(itens == 0){
                    sem_post(&repor);
                    sem_wait(&tempo_repor);
                }
                itens--;
                printf(CYAN "Cliente %d pegou um item. Qtd itens = %d\n" RESET, id, itens);
            pthread_mutex_unlock(&lock);

            //cliente vai para o caixa pagar (se o caixa estiver cheio cliente dorme ate o caixa ficar livre)
            sem_wait(&caixas_livres);
            printf("Cliente %d está indo para o caixa pagar \n",id);
            sem_post(&caixas);

            //apos o pagamento
            sem_wait(&pagamento);
            //cliente vai embora e libera um espaço no caixa
            sem_post(&caixas_livres);
            //e libera um espaco no mercado
            sem_post(&vagas_mercado);
            printf(YELLOW"Cliente %d já efetuou a compra e está indo embora \n" RESET,id);
            
        }
        else{  //merecado cheio
            printf(CLARO "Mercado cheio, cliente %d indo embora\n" RESET,id);
        }
    }
}

void * f_repositor(void *v){
    while(1){
        //repositor esperar os itens acabarem
        sem_wait(&repor);

        //Repositor demora x tempo para repor todos os itens
        sleep(1);
        printf(MAGENTA "Funcionário está repondo os itens\n" RESET); 
        itens = N_ITENS;

        //Funcionario libera o proximo cliente a pegar um item
        sem_post(&tempo_repor);
    }
}

int main() {
    pthread_t thr_clientes[N_CLIENTES], thr_caixas[N_CAIXAS], thr_repositor;
    int i;
    int *id;

    //inicializacao dos semaforos
    sem_init(&vagas_mercado, 0, N_MAX_CLIENTES);
    sem_init(&caixas, 0, 0);
    sem_init(&caixas_livres, 0, N_CAIXAS);
    sem_init(&pagamento, 0, 0);
    sem_init(&repor, 0, 0);
    sem_init(&tempo_repor, 0, 0);

    
    for (i = 0; i < N_CLIENTES; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&thr_clientes[i], NULL, f_cliente, (void *) (id));
    }

    for (i = 0; i < N_CAIXAS; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&thr_caixas[i], NULL, f_caixas, (void*) (id));
    }

    pthread_create(&thr_repositor, NULL, f_repositor, NULL);

    for (i = 0; i < N_CLIENTES; i++){
        pthread_join(thr_clientes[i], NULL);
    }
    
    return 0;
}

