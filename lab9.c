#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>


sem_t meio, fim;
int count_meio;


void * sejaBemvindo(void * arg){
    printf("Seja bem-vindo!\n");
    sem_post(&meio);
    pthread_exit(NULL);
}

void * fiqueAVontade(void * arg){
    sem_wait(&meio);
    printf("Fique a vontade.\n");
    count_meio++;
    if(count_meio == 3) sem_post(&fim);
    else sem_post(&meio);
    pthread_exit(NULL);
}

void * aceitaUmCopoDAgua(void * arg){
    sem_wait(&meio);
    printf("Aceita um copo d'agua?\n");
    count_meio++;
    if(count_meio == 3) sem_post(&fim);
    else sem_post(&meio);
    pthread_exit(NULL);
}

void * senteSePorFavor(void * arg){
    sem_wait(&meio);
    printf("Sente-se por favor.\n");
    count_meio++;
    if(count_meio == 3) sem_post(&fim);
    else sem_post(&meio);
    pthread_exit(NULL);
}

void * volteSempre(void * arg){
    sem_wait(&fim);
    printf("Volte sempre!\n");
    pthread_exit(NULL);
}


int main(){
    pthread_t tid[5];
    
    sem_init(&meio, 0, 0);
    sem_init(&fim, 0, 0);
    count_meio = 0;

    if(pthread_create(&tid[0], NULL, sejaBemvindo, NULL)){
        printf("ERRO -- pthread_create\n");
        return 1;
    }

    if(pthread_create(&tid[1], NULL, aceitaUmCopoDAgua, NULL)){
        printf("ERRO -- pthread_create\n");
        return 1;
    }


    if(pthread_create(&tid[2], NULL, fiqueAVontade, NULL)){
        printf("ERRO -- pthread_create\n");
        return 1;
    }


    if(pthread_create(&tid[3], NULL, senteSePorFavor, NULL)){
        printf("ERRO -- pthread_create\n");
        return 1;
    }


    if(pthread_create(&tid[4], NULL, volteSempre, NULL)){
        printf("ERRO -- pthread_create\n");
        return 1;
    }

    for(int i = 0; i < 5; i++){
        if(pthread_join(tid[i], NULL)){
            printf("ERRO -- pthread_join\n");
            return 2;
        }
    }

    return 0;

}
