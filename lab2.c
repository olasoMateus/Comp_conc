#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include <stdbool.h>
#include "timer.h"

float *mat_1; //matriz 1 de entrada
float *mat_2; //matriz 2 de entrada
float *saida_seq; //matriz de saida sequencial
float *saida_conc; //matriz de saida concorrente
int nthreads; //numero de threads
int dim; //dimensão das matrizes

//funcao que as threads executarao
void * tarefa(void *arg) {
   int id = *(int*) arg;


    for(int i=id; i<dim; i+=nthreads) {
        for(int j = 0; j < dim; j++){
            for(int k = 0; k < dim; k++){
                saida_conc[i*dim + j] += mat_1[i*dim + k]*mat_2[j*dim + k];
            }
        }
    }
    
   pthread_exit(NULL);
}

//função que verifica os valores das saídas
bool funcionando(){
    for(int i = 0; i< dim;i++){
        for(int j = 0;j< dim;j++){
            if(saida_seq[i*dim + j] != saida_conc[i*dim + j]){
                return false;
            }
        }
    }
    return true;
}

int main(int argc, char* argv[]){
    pthread_t *tid; //identificadores das threads no sistema
    double inicio, fim, delta;
   
    GET_TIME(inicio);
    //leitura e avaliacao dos parametros de entrada
    if(argc<3) {
        printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
        return 1;
    }
    dim = atoi(argv[1]);
    nthreads = atoi(argv[2]);
    if (nthreads > dim) nthreads=dim;

    //alocacao de memoria para as estruturas de dados
    mat_1 = (float *) malloc(sizeof(float) * dim * dim);
    if (mat_1 == NULL) {
        printf("ERRO--malloc\n"); 
        return 2;
    }

    mat_2 = (float *) malloc(sizeof(float) * dim * dim);
    if (mat_2 == NULL) {
        printf("ERRO--malloc\n");
        return 2;
    }

    saida_seq = (float *) malloc(sizeof(float) * dim * dim);
    if (saida_seq == NULL) {
        printf("ERRO--malloc\n");
        return 2;
    }

    saida_conc = (float *) malloc(sizeof(float) * dim * dim);
    if (saida_conc == NULL) {
        printf("ERRO--malloc\n");
        return 2;
    }

    //inicializacao das estruturas de dados de entrada e saida
    for(int i=0; i<dim; i++) {
        for(int j=0; j<dim; j++){
            //gerando valores entre 0 e 101
            mat_1[i*dim+j] = rand() % 101;    //equivalente mat_1[i][j]
            mat_2[i*dim+j] = rand() % 101;    //equivalente mat_2[i][j]
            saida_seq[i*dim + j] = 0;
            saida_conc[i*dim + j] = 0;
        }
    }
    //gerando seed para randomizar os valores
    srand(inicio);

    //realizando a multiplicação sequencialmente
    GET_TIME(inicio);
    int r_line = 0, r_col = 0;
    for(int i=0; i<dim; i++) {
        for(int j = 0; j < dim; j++){
            for(int k = 0; k < dim; k++){
                saida_seq[i*dim + j] += mat_1[i*dim + k]*mat_2[j*dim + k];
            }
        }
    }

    GET_TIME(fim)   
    delta = fim - inicio;
    printf("Tempo multiplicacao sequencial:%lf\n", delta);

    //realizando a multiplicação concorrentemente
    GET_TIME(inicio);

    //alocacao das estruturas
    tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
    if(tid==NULL) {
        puts("ERRO--malloc"); return 2;
    }

    //criacao das threads
    for(int i=0; i<nthreads; i++) {
        int a = i;
        if(pthread_create(tid+i, NULL, tarefa, (void*) &a)){
            puts("ERRO--pthread_create"); return 3;
        }
    } 
    //espera pelo termino da threads
    for(int i=0; i<nthreads; i++) {
        pthread_join(*(tid+i), NULL);
    }
    GET_TIME(fim)   
    delta = fim - inicio;
    printf("Tempo multiplicacao concorrente:%lf\n", delta);

    //Verifica se tudo está certo
    

    if(funcionando()){
        printf("worked!\n");
        return 0;
    }
    printf("Algo de errado!\n");
    return 4;
    
}