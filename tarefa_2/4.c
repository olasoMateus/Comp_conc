#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>

int num_thread_impres; //Número de threads pedindo impressões
int pos_impri; // Posição que deve ser imprimida
int pos_entr; // Posição de entrada na fila
int cont; // Número de elementos na fila
int * vetor_impres; // Guarda as impressões
sem_t ha_impressao; //Semáforo que verifica se há impressões a serem feitas, inicia com valor 0
sem_t fila_cheia; //Semáforo que verifica se a fila está cheia, inicia com valor 1 (cheio quando não há sinal)
sem_t e; //Semáforo para uso de varáveis globais

void * gerencia(void * arg){
    while(1){
        sem_wait(&ha_impressao); //Verifica se há impressões a serem feitas
        printf("Imprimiu a requisidao de %d, na posicao %d\n", vetor_impres[pos_impri], pos_impri);
        vetor_impres[pos_impri] = 0; //Consome a impressão na posição a ser imprimida
        pos_impri++; //Atualiza a posição a ser imprimida
        if (pos_impri % num_thread_impres == 0){
            pos_impri = 0;
        }
        sem_wait(&e); // Usa o semáforo de utilizar a variável global
        cont--; // Diminui o contador de número de elementos da fila
        if(cont != 0) sem_post(&ha_impressao); //Se ainda há elementos na fila, faz um post para imprimir novamente
        if(cont == num_thread_impres - 1) sem_post(&fila_cheia); // Se a fila não está cheia, (ou seja, há espaço para novas requisições), faz um post para requesições
        sem_post(&e); // Para de utilizar a variável global
    }

    pthread_exit(NULL);
}

void * requisita(void * arg){
    int *id = (int*) arg;
    while(1){
        sem_wait(&fila_cheia); // Verifica se a fila não está cheia
        vetor_impres[pos_entr] = *id; // Bota, na fila, o valor de seu id, para termos um tracking da ordem
        pos_entr++; // Atualiza a posição de entrada da fila de novas requisições de impressão
        if (pos_entr % num_thread_impres == 0){
            pos_entr = 0;
        }
        sem_wait(&e); // Usa o semáforo de utilizar a variável global
        cont++; // Aumenta o contador de número de elementos da fila
        if(cont == 1) sem_post(&ha_impressao); // Se é o primeiro a entrar na fila, manda um sinal para o gerenciador começar a imprimir
        if(cont < num_thread_impres) sem_post(&fila_cheia); // Se não é o último a entrar na fila, sinaliza que ainda há espaço para entrar na fila de impressões
        sem_post(&e); // Para de utilizar a variável global
    }
}

int main(int argc, char* argv[]){
    if(argc < 2){
        printf("Iniciacao no molde - '%s <n de threads de impressao> '\n", argv[0]);
        return 1;
    }
    num_thread_impres = atoi(argv[1]);
    pthread_t *tid;
    int * id;
    pos_impri = 0;
    pos_entr = 0;

    sem_init(&e, 0, 1);
    sem_init(&ha_impressao, 0, 0);
    sem_init(&fila_cheia,0, 1);

    tid = (pthread_t *) malloc(sizeof(pthread_t) * (num_thread_impres + 1));
    if(tid == NULL){
        printf("ERRO -- malloc\n");
        return 2;
    }
    id = (int *) malloc(sizeof(int) * num_thread_impres);
    if(id == NULL){
        printf("ERRO -- malloc\n");
        return 2;
    }
    vetor_impres = (int *) malloc(sizeof(int) * num_thread_impres);
    if(vetor_impres == NULL){
        printf("ERRO -- malloc\n");
        return 2;
    }

    for(int i = 0; i< num_thread_impres; i++){
        vetor_impres[i] = 0;
    }

    if(pthread_create(tid, NULL, gerencia, NULL)){
            printf("ERRO--pthread_create\n");
            return 3;
        }
    for(int i = 0; i <  num_thread_impres - 1;i++){
        *(id + i) = i + 1;
        if(pthread_create(tid + i + 1, NULL, requisita, (void *) (id + i))){
            printf("ERRO--pthread_create\n");
            return 3;
        }
    }

    for(int i = 0; i< num_thread_impres + 1; i++) {
      pthread_join(*(tid+i), NULL);
    }

   return 0;
}