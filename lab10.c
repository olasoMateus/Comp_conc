#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>


int n_prod, n_cons, prox_consumida, tamanho; //Número de produtores, número de consumidores, proximo a ser consumido, tamanho do vetor
int * vetor; // Vetor de itens a serem preenchidos e consumidos
sem_t prior; // Semáfaro de controle de 

void * produtora(void * arg){
    int *id = (int*) arg;
    while(1){
        printf("Produtor %d espera prioridade\n", *id);
        sem_wait(&prior); // Verifica se pode pegar processamento
        printf("Produtor %d pega prioridade\n", *id);
        for(int i = prox_consumida; i < tamanho; i++){
            vetor[i] = 1; // Preenche o vetor
        }
        prox_consumida = tamanho - 1; // Atualiza a próxima posição a ser consumida
        printf("Produtor %d preencheu todas as posicoes e vai liberar alguem\n", *id);
        sem_post(&prior); // Da processamento a uma próxima thread
    }

    pthread_exit(NULL);
}

void * consumidora(void * arg){
    int *id = (int*) arg;
    while(1){
        printf("Consumidor %d espera prioridade\n", *id);
        sem_wait(&prior); // Verifica se pode pegar processamento
        printf("Consumidor %d pega prioridade\n", *id);
        if(prox_consumida == 0){ // Confere se há alguma coisa para ser lida
            printf("Nao ha nada para consumir, passando\n");
        }
        else{
            vetor[prox_consumida] = 0; // Consome o item a ser lido
            printf("Consumidor %d consumiu a posicao %d  e vai liberar alguem\n", *id, prox_consumida);
            prox_consumida--; // Atualiza a próxima posição a ser consumida
        }
        sem_post(&prior); // Da processamento a uma próxima thread
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    if(argc < 4){
        printf("Iniciacao no molde - '%s <n de produtores> <n de consumidores> <tamanho do vetor>'\n", argv[0]);
        return 1;
    }
    n_prod = atoi(argv[1]);
    n_cons = atoi(argv[2]);
    tamanho = atoi(argv[3]);
    pthread_t *tid;
    int * id;
    prox_consumida = 0;

    sem_init(&prior, 0, 1);
    //sem_init(&cons, 0, 0);

    tid = (pthread_t *) malloc(sizeof(pthread_t) * (n_cons + n_prod));
    if(tid == NULL){
        printf("ERRO -- malloc\n");
        return 2;
    }
    id = (int *) malloc(sizeof(int) * (n_cons + n_prod));
    if(id == NULL){
        printf("ERRO -- malloc\n");
        return 2;
    }
    vetor = (int *) malloc(sizeof(int) * tamanho);
    if(vetor == NULL){
        printf("ERRO -- malloc\n");
        return 2;
    }

    for(int i = 0; i< tamanho; i++){
        vetor[i] = 0;
    }

    for(int i = 0; i <  n_prod;i++){
        *(id + i) = i;
        if(pthread_create(tid + i, NULL, produtora, (void *) (id + i))){
            printf("ERRO--pthread_create\n");
            return 3;
        }
    }

    for(int i = n_prod; i < (n_prod + n_cons);i++){
        *(id + i) = i - n_prod;
        if(pthread_create(tid + i, NULL, consumidora, (void *) (id + i))){
            printf("ERRO--pthread_create\n");
            return 3;
        }
    }

    for(int i = 0; i< (n_prod + n_cons); i++) {
      pthread_join(*(tid+i), NULL);
    }

   return 0;
}

