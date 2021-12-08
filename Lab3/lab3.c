#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"

long long int N;//Dimensão do vetor
int nthread;//Número de threads
int limInf; //Limite inferior
int limSup; //Limite superior
float *vetor; //Vetor de valores float aleatórios
long long int bloco; //Tamanho dos blocos que as threads iram percorrer

void * tarefa_somar(void * arg){
    long int id = (long int) arg;

    //Salva os valores de inicio e fim das partes do vetor que se deve buscar
    long long int inicio = id*bloco;
    long long int fim;
    if(id == nthread - 1) fim = N;
    else fim = inicio + bloco;

    //Cria a variável de retorno
    int *retorno;

    retorno = (int *) malloc(sizeof(int));
    if(retorno == NULL){
        fprintf(stderr, "ERRO -- malloc (dentro da thread)\n");
        pthread_exit(NULL);
    }

    for(long long int i = inicio; i < fim;i++){
        if(vetor[i] > limInf && vetor[i] < limSup) *retorno += 1;
    } 

    //retorna o valor somado dentro da thread
    pthread_exit((void *) retorno);
}




int main(int argc, char * argv[]){
    int totSeq = 0, totConc = 0;
    double inicio, fim, delta;
    pthread_t *tid; //identificadores das threads no sistema
    int *retorno; //valor de retorno das threads

    if(argc < 5){
        fprintf(stderr,"Digite: %s <Tamanho do vetor> <Numero de threads> <Limite inferior> <Limite superior>\n", argv[0]);
        return 1;
    }
    N = atoll(argv[1]);
    nthread = atoi(argv[2]);
    limInf = atoi(argv[3]);
    limSup = atoi(argv[4]);
    bloco = N/nthread; //tamanho do bloco: tamanho do vetor/Nº de threads

    vetor = (float *) malloc(sizeof(float) * N);
    if(vetor == NULL){
        fprintf(stderr, "ERRO -- malloc!\n");
        return 2;
    }

    //preencher o vetor com valores aleatórios

    //Mundando a seed do randomizador
    GET_TIME(inicio);

    srand(inicio);

    for(long long int i = 0; i < N; i++){
        vetor[i] = (float) (rand() % (limSup)*2) + (float)(i % 100)/100;
    }

    //fazer a soma sequencial
    GET_TIME(inicio);

    
    for(long long int i = 0; i < N; i++){
        if(vetor[i] > limInf && vetor[i] < limSup) totSeq++;
    }

    GET_TIME(fim);
    delta = fim - inicio;

    printf("Tempo sequencial: %lf\n", delta);


    //fazer a soma concorrênte

    GET_TIME(inicio);
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthread);
    if(tid == NULL){
        fprintf(stderr, "ERRO -- malloc!\n");
        return 2;
    }

    //chamar as threads
    for(long int i = 0; i < nthread ; i++){
        if(pthread_create(&tid[i], NULL,tarefa_somar, (void *) i)){
            fprintf(stderr, "ERRO -- pthread_create\n");
            return 3;
        }
    }

    //espera as threads acabar e soma tudo na somaConc
    for(int i = 0; i < nthread; i++){
        if(pthread_join(tid[i], (void **) &retorno)){
            fprintf(stderr, "ERRO -- pthread_join\n");
            return 4;
        }
        totConc += *retorno;
        free(retorno);
    }

    GET_TIME(fim);
    delta = fim - inicio;

    printf("Tempo concorrente: %lf\n", delta);

    //exibe os resultados
    printf("Total sequencial: %d\n", totSeq);
    printf("Total concorrente: %d\n", totConc);
    if(totConc == totSeq) printf("Tudo certo!\n");
    else fprintf(stderr, "Algo deu errado!");
    
    free(tid);
    free(vetor);

    return 0;

}