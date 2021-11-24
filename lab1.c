// Nome: Mateus Ferreira Olaso
//DRE: 120056682

//Tentei criar um modelo, que serve de base para a mesma questão com outros tamanhos e
//números de threads

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define TAM 1000 // Tamanho do vetor
#define NUM_THREADS 2 // Número de threads criadas; Precisa ser multiplo de TAM

typedef struct _val{
    int inicial;//Valor inicial de mudança no vetor
    int final;//Valor final de mudança no vetor
    int order_thread;//Número de chamada da thread
} valores;

int vetor_resp[TAM];

void * elevaAoQuadrado (void * arg){
    valores * val = (valores *) arg;

    printf("Sou a thread %d de %d threads\n", val->order_thread, NUM_THREADS);
    for(int i = val->inicial; i < val->final;i++){
        vetor_resp[i] = vetor_resp[i]*vetor_resp[i];//Elevando o que estão no vetor na pos. i ao quadrado
    }

    free(val);
    pthread_exit(NULL);
}

int main(){

    pthread_t tid_sistema[NUM_THREADS]; //identificadores das threads no sistema
    valores *arg; //receberá os argumentos para a thread
    int size_thread = TAM/NUM_THREADS; // Número de valores que cada thread executa

    for(int i = 0; i< TAM; i++){
        vetor_resp[i] = i;
    }

    for(int i = 0; i < NUM_THREADS; i++){
        arg = malloc(sizeof(valores));
        if (arg == NULL) {
            printf("Erro de espaço de memória"); exit(-1);
        }
        arg->inicial = i*size_thread;
        arg->final = (i + 1)*size_thread;
        arg->order_thread = i;

        printf("--Cria a thread %d\n", i);
        if (pthread_create(&tid_sistema[i], NULL, elevaAoQuadrado, (void*) arg)) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }

    //--espera todas as threads terminarem
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(tid_sistema[i], NULL)) {
            printf("--ERRO: pthread_join() \n"); exit(-1); 
        } 
    }

    //Testar se está tudo correto
    for(int i = 0; i < TAM; i++){
        if(vetor_resp[i] != i*i){
            printf("Erro - Teste invalido: %d e %d\n", vetor_resp[i], i*i);
            exit(-1);
        }
    }

    printf("--Thread principal terminou - Sucesso\n");
    pthread_exit(NULL);

}