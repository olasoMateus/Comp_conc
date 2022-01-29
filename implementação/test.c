#include<stdio.h>
#include<stdlib.h>

typedef struct vertice{
    int id_vertice;
    struct vertice * prox;
}VERTICE;

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

void DFS(int id){

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
}

int main(){
    int p, q;

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

    for(int i = 0; i < num_de_vertices; i++){
        V[i] = NULL;
        visitados[i] = 0;
    }

    printf("Entre com uma aresta 'p q' (digite '0 0', '1 1' para encerrar as entradas): \n");
    scanf("%d %d", &p, &q);
    while(p != q){
        insere(p, q);
        printf("Entre com uma aresta 'p q' (digite '0 0', '1 1', etc... para encerrar as entradas): \n");
        scanf("%d %d", &p, &q);
    }

    DFS(0);

    printf("Qual vertice voce quer verificar se foi achado? \n");
    scanf("%d", &p);

    if( p >= num_de_vertices || p < 0) printf("Esse vertice não existe!\n");

    else if (visitados[p] == 1) printf("Ele foi achado!\n");

    return 0;

}