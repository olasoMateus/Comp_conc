#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

typedef struct vertice{
    int id_vertice;
    struct vertice * prox;
}VERTICE;

int nthread;

int num_elementos_lidos;

int prox_a_ser_lido;

int * espera_leitura;

pthread_cond_t cond; //Variavel de condição
pthread_mutex_t mutex; //Variavel de lock e unlock

int * visitados;

int num_de_vertices;

VERTICE * * V;

void insere(int p, int q){
    VERTICE *vi, *vj;

    vj = (VERTICE *) malloc(sizeof(VERTICE));
    vj->id_vertice = q;
    vj->prox = NULL;

    if(V[p] == NULL){
        V[p] = vj;
    }
    else{
        vi = V[p];
        while(vi->prox != NULL){
            vi = vi->prox; 
        }
        vi->prox = vj;
    }

    return;
}

/*void DFS(int id){

    VERTICE *v;
    int id_prox;
    
    visitados[id] = 1;
    v = V[id];

    printf("%d achado! \n", id);

    while(v != NULL){
        id_prox = v->id_vertice;
        if(!visitados[id_prox]){
            DFS(id_prox);
        }
        v = v->prox;
    }

    return;
}*/

void * thread_dfs(void * arg){
    int * id_thread = (int *) arg;

    VERTICE *v = NULL;
    int id_prox = 0;

    while(num_elementos_lidos != num_de_vertices){
        pthread_mutex_lock(&mutex);
        id_prox = espera_leitura[prox_a_ser_lido];
        espera_leitura[prox_a_ser_lido] = -1;
        prox_a_ser_lido--;
        num_elementos_lidos++;
        pthread_mutex_unlock(&mutex);

        visitados[id_prox] = 1;
        v = V[id_prox];

        printf("%d achado pela thread %d! \n", id_prox, *id_thread);

        while(v != NULL){
            id_prox = v->id_vertice;
            if(!visitados[id_prox]){
                pthread_mutex_lock(&mutex);
                prox_a_ser_lido++;
                espera_leitura[prox_a_ser_lido] = id_prox;
                pthread_mutex_unlock(&mutex);
            }
            v = v->prox;
        }
    }

    pthread_exit(NULL);


}

int main(){
    int p, q;
    int * id;

    pthread_t *tid; //identificadores das threads no sistema

    nthread = 4;
    prox_a_ser_lido = 0;
    num_elementos_lidos = 0;

    printf("Digite o numero de vertices: \n");
    scanf("%d", &num_de_vertices);
    
    V = (VERTICE **) malloc(sizeof(VERTICE) * num_de_vertices);
    if(V == NULL){
        printf("ERRO --- MALLOC\n");
        return 1;
    }

    visitados = (int *) malloc(sizeof(int) * num_de_vertices);
    if(visitados == NULL){
        printf("ERRO --- MALLOC\n");
        return 1;
    }

    espera_leitura = (int *) malloc(sizeof(int) * num_de_vertices);
    if(espera_leitura == NULL){
        printf("ERRO --- MALLOC\n");
        return 1;
    }

    id = (int *) malloc(sizeof(int)*nthread);
    if(id == NULL){
        puts("Erro--malloc"); return 2;
    }



    for(int i = 0; i < num_de_vertices; i++){
        V[i] = NULL;
        visitados[i] = 0;
        espera_leitura[i] = -1;
    }

    printf("Entre com uma aresta 'p q' (digite '0 0', '1 1' para encerrar as entradas): \n");
    scanf("%d %d", &p, &q);
    while(p != q){
        insere(p, q);
        printf("Entre com uma aresta 'p q' (digite '0 0', '1 1', etc... para encerrar as entradas): \n");
        scanf("%d %d", &p, &q);
    }

    espera_leitura[0] = 0;

    tid = (pthread_t*) malloc(sizeof(pthread_t)*nthread);
    if(tid==NULL) {
        puts("ERRO--malloc"); return 2;
    }

    //criacao das threads
    for(int i=0; i<nthread; i++) {
        *(id + i) = i;
        if(pthread_create(tid+i, NULL, thread_dfs, (void*) (id + i))){
            puts("ERRO--pthread_create"); return 3;
        }
    } 
    //espera pelo termino da threads
    for(int i=0; i<nthread; i++) {
        pthread_join(*(tid+i), NULL);
    }

    printf("Qual vertice voce quer verificar se foi achado? \n");
    scanf("%d", &p);

    if( p >= num_de_vertices || p < 0) printf("Esse vertice não existe!\n");

    else if (visitados[p] == 1) printf("Ele foi achado!\n");

    return 0;

}