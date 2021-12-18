#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS  5

/* Variaveis globais */
int x = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

void * sejaBemVindo(void * id){

    pthread_mutex_lock(&x_mutex);
    while(x != 0){
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    x++;
    printf("Seja bem-vindo!\n");
    pthread_cond_signal(&x_cond);
    pthread_mutex_unlock(&x_mutex);
    pthread_exit(NULL);
}

void * volteSempre(void * id){
    pthread_mutex_lock(&x_mutex);
    while(x != 4){
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    printf("Volte sempre!\n");
    pthread_mutex_unlock(&x_mutex);

    pthread_exit(NULL);

}

void * fiqueAVontade(void * id){
    
    pthread_mutex_lock(&x_mutex);
    while(x == 0){
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    x++;
    printf("Fique a vontade.\n");
    pthread_cond_signal(&x_cond);
    pthread_mutex_unlock(&x_mutex);

    pthread_exit(NULL);

}

void * senteSePorFavor(void * id){
    
    pthread_mutex_lock(&x_mutex);
    while(x == 0){
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    x++;
    printf("Sente-se por favor.\n");
    pthread_cond_signal(&x_cond);
    pthread_mutex_unlock(&x_mutex);
    
    pthread_exit(NULL);

}

void * aceitaUmCopoDAgua(void * id){
    
    pthread_mutex_lock(&x_mutex);
    while(x == 0){
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    x++;
    printf("Aceita um copo d'agua?\n");
    pthread_cond_signal(&x_cond);
    pthread_mutex_unlock(&x_mutex);
    
    pthread_exit(NULL);

}

int main(){
    pthread_t threads[NTHREADS];
    int * t1,* t2,* t3,* t4,* t5;

    //aloca espaco para os identificadores das threads
    t1=malloc(sizeof(int)); *t1 = 1;
    t2=malloc(sizeof(int)); *t2 = 2;
    t3=malloc(sizeof(int)); *t3 = 3;
    t4=malloc(sizeof(int)); *t4 = 4;
    t5=malloc(sizeof(int)); *t5 = 5;

    /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
    pthread_mutex_init(&x_mutex, NULL);
    pthread_cond_init (&x_cond, NULL);

    if(pthread_create(&threads[0], NULL, sejaBemVindo, (int *) t1)){
        printf("ERRO -- pthread_create\n");
        return 1;
    };

    if(pthread_create(&threads[1], NULL, aceitaUmCopoDAgua, (int *) t2)){
        printf("ERRO -- pthread_create\n");
        return 1;
    };

    if(pthread_create(&threads[2], NULL, senteSePorFavor, (int *) t3)){
        printf("ERRO -- pthread_create\n");
        return 1;
    };

    if(pthread_create(&threads[3], NULL, fiqueAVontade, (int *) t4)){
        printf("ERRO -- pthread_create\n");
        return 1;
    };

    if(pthread_create(&threads[4], NULL, volteSempre, (int *) t5)){
        printf("ERRO -- pthread_create\n");
        return 1;
    };

    /* Espera todas as threads completarem */
    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf ("\nFIM.\n");

    /* Desaloca variaveis e termina */
    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond);

    return 0;
}
