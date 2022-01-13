#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int nthread; //Número de threads
int contador; //Contador das trheads para a barreira
int * vetor; //Vetor utilizado pelas threads
pthread_cond_t cond; //Variavel de condição
pthread_mutex_t mutex; //Variavel de lock e unlock

//Verifica se todas athreads já contaram os valores do vetor
void barreira(){
    pthread_mutex_lock(&mutex); //Faz um lock para verificar as condições de barreira
    if((contador - 1) == 0){ //Caso a thread seja a última a contar os valores do vetor
            pthread_cond_broadcast(&cond); //Sendo a thread a última, libera todas as threads e retorna o contador ao seu valor inicial
            contador = nthread;
    }
    else{
            contador--; //Caso não seja a última, diminui do contador e entra em espera
            pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex); //Faz o unlock da thread
    return;
}


void * tarefa(void * arg){
    int * id = (int *)  arg;
    int *total;

    total = (int *) malloc(sizeof(int));//inicializa a variavel de retorno
    if(total == NULL){
        fprintf(stderr, "ERRO -- Malloc\n");
        return arg;
    }
    *total = 0;
    for(int i = 0; i<nthread;i++){
        for(int j = 0; j < nthread; j++){ //Faz o somatório dos valores do vetor
            *total += vetor[j];
        }
        barreira(); //Chama a condição de barreira, para não modificar o vetor sem todas as threads terem o lido antes
        
        vetor[*id] = rand() % 10; //Atualiza o vetor na posição do id da thread
        barreira(); //Chama a barreira, para prosseguir após a mudança de valores do vetor de todas as posições serem feitas
    }
    pthread_exit((void *) total); //Retorna o somátorio total do vetor e suas atualizações
}


int main(int argc, char * argv[]){
    if(argc < 2){ //Confere se a inicialização foi feita de maneira correta
        fprintf(stderr,"Digite: %s <Numero de threads> \n", argv[0]);
        return 1;
    }
    nthread = atoll(argv[1]); 
    contador = nthread;
    pthread_t *tid;
    int * retorno;
    int * auxilio;
    int id[nthread];

    vetor = (int *) malloc(sizeof(int) * nthread);
    if(vetor == NULL){
        fprintf(stderr,"ERRO -- Mallloc\n");
        return 2;
    }

    retorno = (int *) malloc(sizeof(int) * nthread);
    if(retorno == NULL){
        fprintf(stderr,"ERRO -- Mallloc\n");
        return 2;
    }


    for(int i = 0; i < nthread;i++){//Preenche o vetor pela primeira vez
        vetor[i] = rand() % 10;
    }

    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthread);
    if(tid == NULL){
        fprintf(stderr,"ERRO -- Mallloc\n");
        return 2;
    }

    //chamar as threads
    for(int i = 0; i < nthread ; i++){
        id[i] = i;
        if(pthread_create(&tid[i], NULL,tarefa, (void *) &id[i])){
            fprintf(stderr, "ERRO -- pthread_create\n");
            return 2;
        }
    }

    //Espera as threads e armazena os valores de retorno delas
    for(int i = 0; i < nthread; i++){
        if(pthread_join(tid[i], (void **) retorno[i])){
            fprintf(stderr, "ERRO -- pthread_join\n");
            return 2;
        }
    }

    for(int i = 0; i < nthread ; i++){ //Verifica se os valores do somatório batem
        if(retorno[i] != retorno[0]){
            fprintf(stderr, "Algo deu errado! -- %d -- %d -- %d\n", retorno[i], retorno[0], i);
            return 3;
        }
    }

    printf("Tudo certo!!\n");

    free(vetor);
    free(tid);
    free(retorno);

    return 0;

}
