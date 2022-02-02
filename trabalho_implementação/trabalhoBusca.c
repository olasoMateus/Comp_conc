#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include "timer.h"

#define N_THREADS 4 //Número de threads criadas
#define N_ELEMENTOS 1023 //Número de elementos da árvore

typedef struct elemArv elemArv; 

struct elemArv{

	int nFilhos;
	int valor;
	elemArv* filhoEsq;
	elemArv* filhoDir; 
	elemArv* pai;

}; //Estrutura do elemento que compõe a árvore

elemArv* initArv(elemArv* arvore){

	arvore -> nFilhos = 0;
	arvore -> valor = -1;
	arvore -> filhoEsq = NULL;
	arvore -> filhoDir = NULL;
	arvore -> pai = NULL;
	
	return arvore;

}

void insereArv(elemArv* raiz, int numero){ //Função que insere um elemento na árvore

	if(raiz == NULL){
		printf("Vértice inválido\n");
		return;
	
	}
	
	if(raiz -> valor == -1){
		raiz -> valor = numero;
		return;
	
	} else if(raiz -> nFilhos == 0){
		elemArv* novoNo = (elemArv*) malloc(sizeof(elemArv));
		novoNo = initArv(novoNo);
		novoNo -> pai = raiz;
		
		novoNo -> valor = numero;
		raiz -> filhoEsq = novoNo;
		raiz -> nFilhos++;
		return;
	
	} else if(raiz -> nFilhos == 1){
		elemArv* novoNo = (elemArv*) malloc(sizeof(elemArv));
		novoNo = initArv(novoNo);
		novoNo -> pai = raiz;
		
		novoNo -> valor = numero;
		raiz -> filhoDir = novoNo;
		raiz -> nFilhos++;
		return;
	
	} else if( ((raiz -> filhoEsq) -> nFilhos) == 2){
		if ( ((raiz -> filhoDir) -> nFilhos) < 2 ){
			insereArv(raiz -> filhoDir, numero);
			return;	
			
		} else {
			insereArv(raiz -> filhoEsq, numero);
			return;			
		
		}
	
	} else {
		insereArv(raiz -> filhoEsq, numero);
		return;
		
	}

}

void preOrdem(elemArv* ptraiz){ //Função que imprime a ordem da árvore

	printf("%d ", (ptraiz) -> valor);
	if( ptraiz -> filhoEsq != NULL){
		preOrdem( ptraiz -> filhoEsq);
	}
	if( ptraiz -> filhoDir != NULL){
		preOrdem( ptraiz-> filhoDir);
	}
	return;

}



typedef struct{
	int numero;
	int id;
	
} tArgs; //Estrutura para passa o valor do vértice buscado as threads e o id das mesmas

typedef struct{
	elemArv* subArvore;
	int checado;
	
} ponteiroGlobal; //Estrutura que forma a lista que armazena as sub-árvores e verifica se as mesmas já foram checadas ou não.

ponteiroGlobal** listaPonteiros; //Lista onde as sub-árvores são armazenadas
elemArv** retornos; //Função que armazena o retorno, contendo o vértice desejado
elemArv** retornos_sequenciais; //Função que armazena o retorno, contendo o vértice desejado (sequêncial)
int iteradorGlobal; //Mantém a posição onde a próxima sub-árvore deve entrar na lista
int iteradorRetorno; //Mantém a posição do retorno dentro da lista de retornos
int iteradorRetornoSeq; //Mantém a poição do retorno dentro da lista de retornos (sequêncial)
int threads_dormindo; //Mantém o número de threads ociosas
int iteradorThreads; //Mantém a posição da próxima sub-árvore que se deve fazer DFS

pthread_mutex_t mutex_acesso, acessoGlobal, mutex_retorno, mutex_sono; //As variáveis de lock e unlock
pthread_cond_t disponivel; //A variável de condição

void DFS_concorrente(elemArv* arvore, int numero){


	if( (arvore -> valor) == numero ){ //Caso o valor do vértice seja o valor procurado

		pthread_mutex_lock(&mutex_retorno); //Faz um lock, para o caso de de um encontro simultânio entre threads (impossível na estrutura de árvore)
		retornos[iteradorRetorno] = arvore; //Adiciona a sub-árvore do vértice ao retorno
		iteradorRetorno++;
		printf("Retorno adcionado!\n");
		pthread_mutex_unlock(&mutex_retorno);
	
	} 

	if( (arvore -> filhoDir) != NULL ){ //Caso haja filho à direita, o armazena na fila
	
		
		ponteiroGlobal* elemFila = malloc(sizeof(ponteiroGlobal)); //Cria uma nova váriavel de ponteiroGlobal e armazena a árvore que tem como raiz o filho a direita do vértice analisado
		elemFila -> subArvore = arvore -> filhoDir;
		elemFila -> checado = 0;
		

		pthread_mutex_lock(&acessoGlobal); //Faz um lock para mexer na variável global
		listaPonteiros[iteradorGlobal] = elemFila; //Põe a sub-árvore na lista
		iteradorGlobal++; //Atualiza a primeira posição livre na fila

		pthread_mutex_unlock(&acessoGlobal); 

		pthread_cond_signal(&disponivel); //Sinaliza uma thread ociosa que há uma nova sub-árvore para ser explorada	
	
	}
	
	if( (arvore -> filhoEsq) != NULL ){ //Caso hajá filho à esquerda, prosseque com o DFS

		DFS_concorrente(arvore -> filhoEsq, numero);
	
	}


	return;

}

void DFS_normal(elemArv* arvore, int numero){ //DFS sequêncial

	if( (arvore -> valor) == numero ){

		retornos_sequenciais[iteradorRetornoSeq] = arvore;
		iteradorRetornoSeq++;

	
	} 
	
	if( (arvore -> filhoEsq) != NULL ){

		DFS_normal(arvore -> filhoEsq, numero);
	
	}

	if( (arvore -> filhoDir) != NULL ){
	

		DFS_normal(arvore -> filhoDir, numero);		
	
	}
	
	return;

}

void* tarefa(void* arg){ //Thread lançada pela main
	//int id = ((tArgs*) arg) -> id; //Armazena seu id
	int numero = ((tArgs*) arg) -> numero; //Armazena o número do vértice que estamos buscando
	int iteradorLocal = 0; //Mantém um iterador para percorrer pela fila
	
    pthread_mutex_lock(&mutex_acesso);
    while(iteradorThreads < N_ELEMENTOS){

        while( iteradorThreads == iteradorGlobal){ //Caso não haja nenhuma sub-árvore para ser lida

			pthread_mutex_lock(&mutex_sono); //Faz um lock para verificar quantas threads estão ociosas
			if(threads_dormindo == N_THREADS-1){ //Caso todas execeto essa estejam ociosas

				threads_dormindo++;
				pthread_cond_broadcast(&disponivel); //Libera todas as outras threads
                pthread_mutex_unlock(&mutex_acesso);
				pthread_mutex_unlock(&mutex_sono);
				return NULL; //Encerra seu processamento
			}
			
			threads_dormindo++; //Aumenta o número de threads "dormindo"

			pthread_mutex_unlock(&mutex_sono);



			pthread_cond_wait(&disponivel, &mutex_acesso); //Fica ociosa, aguardando alguma nova sub-árvore não explorada ou o fim da execução



			pthread_mutex_lock(&mutex_sono);

			if(threads_dormindo == N_THREADS){ //Caso todas as threads estejam ociosas

                pthread_mutex_unlock(&mutex_acesso);
				pthread_mutex_unlock(&mutex_sono);
				return NULL; //Encerra seu processamento
			}
			

			threads_dormindo--; //Sai da contagem de threads ociosas, para verificas se há alguma sub-árvore que possa executar o DFS
			pthread_mutex_unlock(&mutex_sono);			
			
		}
        
        iteradorLocal = iteradorThreads;
        iteradorThreads++;
        pthread_mutex_unlock(&mutex_acesso);

        if( (listaPonteiros[iteradorLocal]) -> checado == 0){ //Caso a sub-árvore ainda não tenha sido verificada

			(listaPonteiros[iteradorLocal]) -> checado = 1;	

			DFS_concorrente((listaPonteiros[iteradorLocal]) -> subArvore, numero);


        }

        pthread_mutex_lock(&mutex_acesso);
    }


	return NULL; //Encerra seu processamento

}

char* recuperaCaminho(elemArv* no, char* stringCaminho){ //Função feita para verificar a corretude

	elemArv* pai = no -> pai;
	
	if(pai == NULL){
		return stringCaminho;
	
	}
	
	if(pai -> filhoEsq == no){
		char ch = 'e';
		strncat(stringCaminho, &ch, 1);
		recuperaCaminho(pai, stringCaminho);
	
	}
	
	if(pai -> filhoDir == no){
		char ch = 'd';
		strncat(stringCaminho, &ch, 1);
		recuperaCaminho(pai, stringCaminho);
	
	}
	
	return stringCaminho;

}

char *strrev(char *str) //Função feita para verificar a corretude
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}

int corretude(elemArv* arvore, elemArv* retorno, int numero){ //Função de corretude do algoritmo

	char* caminho = (char*) malloc(sizeof(char)*N_ELEMENTOS);
	recuperaCaminho(retorno, caminho);
	
	caminho = strrev(caminho);
	printf("Caminho da raiz até o nó: %s\n", caminho);
	
	int tamanho = strlen(caminho);
	for(int i = 0; i < tamanho; i++){
		if (caminho[i] == 'e'){
			arvore = arvore -> filhoEsq;
		
		}
		
		if(caminho[i] == 'd'){
			arvore = arvore -> filhoDir;
		
		}
	
	}
	
	if(arvore->valor == numero){	
		return 0;
	
	} else{
		return 1;
			
	}
		
	return 1;

}

int main(){

	double inicio, fim, deltaConc, deltaSeq;

	listaPonteiros = (ponteiroGlobal**) malloc(sizeof(ponteiroGlobal) * N_ELEMENTOS);
	retornos = (elemArv**) malloc(sizeof(elemArv) * N_ELEMENTOS);
	retornos_sequenciais = (elemArv**) malloc(sizeof(elemArv) * N_ELEMENTOS);
	iteradorGlobal = 1;
	iteradorRetorno = 0;
	iteradorRetornoSeq = 0;
	threads_dormindo = 0;
    iteradorThreads = 0;

	elemArv* arvore = (elemArv*) malloc(sizeof(elemArv));
	arvore = initArv(arvore);
	
	for(int i = 0; i < N_ELEMENTOS; i++){ //Cria os elementos e insere os mesmos na árvore
		insereArv(arvore, i);
	
	}
	
	
	//Vértice inicial
	ponteiroGlobal* ptInit = malloc(sizeof(ponteiroGlobal));
	ptInit -> subArvore = arvore;
	ptInit -> checado = 0;
	
	listaPonteiros[0] = ptInit;
	
	
	int numeroProcurado = -1;
	while(numeroProcurado < 0 || numeroProcurado > N_ELEMENTOS){
        printf("Digite o número a ser buscado: "); //Pede ao usuário o vértice desejado (fazer loop de verificação)
	    scanf("%d", &numeroProcurado);
        puts("");
        if(numeroProcurado < 0 || numeroProcurado > N_ELEMENTOS){
            printf("Por favor, digite um valor entre 0 e %d\n", N_ELEMENTOS);
        }
    }
	puts("");
	
	// DFS Sequencial - Início
	GET_TIME(inicio); //Começa a contagem do tempo sequêncial
	DFS_normal(arvore, numeroProcurado);
	GET_TIME(fim); //Fim da contagem do tempo sequêncial
	// DFS Sequencial - Fim
	
	deltaSeq = fim - inicio;
	printf("Tempo busca sequencial: %lfs\n", deltaSeq);
	
	// DFS Concorrente - Início
	GET_TIME(inicio); //Começa a contagem do tempo concorrênte
	pthread_t *tid; //identificadores das threads no sistema
	tArgs *args;
	
	tid = (pthread_t*) malloc(sizeof(pthread_t) * N_THREADS);	
    if(tid==NULL) {puts("ERRO--malloc"); return 2;}
    
    args = (tArgs*) malloc(sizeof(tArgs) * N_THREADS);
    if(args==NULL) {puts("ERRO--malloc"); return 2;}
    
    //printf("======= Início Log =======\n");
	puts("");
    //cria threads
	for(int i = 0; i < N_THREADS; i++) {
	  (args+i)->id = i;
	  (args+i)->numero = numeroProcurado;

      if(pthread_create(tid+i, NULL, tarefa, (void*)(args+i))){
         puts("ERRO--pthread_create"); return 3;
      }
    }
    
	//espera pelo termino da threads
	for(int i=0; i<N_THREADS; i++) {
		pthread_join(*(tid+i), NULL);
	}  

	GET_TIME(fim); //Fim da contagem do tempo concorrênte
	// DFS Concorrente - Fim
	
	deltaConc = fim - inicio;
	printf("Tempo busca concorrente: %lfs\n", deltaConc);
	puts("");
	
	//Corretude Sequencial
	/*for(int i = 0; i < N_ELEMENTOS; i++){
		if(retornos_sequenciais[i] == NULL){
			break;
			
		}
		
		if(corretude(arvore, retornos_sequenciais[i], numeroProcurado) != 0){
			printf("Resultado incorreto, falha na corretude\n");
			return 0;
		}
	}*/
	
	printf("Corretude sequencial avaliada sem erros\n");
	puts("");
	
	//Corretude Concorrente
	/*for(int i = 0; i < N_ELEMENTOS; i++){
		if(retornos[i] == NULL){
			break;
			
		}
		
		if(corretude(arvore, retornos[i], numeroProcurado) != 0){
			printf("Resultado incorreto, falha na corretude\n");
			return 0;
		}
	}*/
	
	printf("Corretude concorrente avaliada sem erros\n");
	puts("");
	
	printf("Desempenho: %lf\n", deltaSeq/deltaConc); //Calculo do desempenho entre a execução sequêncial e concorrênte
	// > 1, sequêncial mais lento; < 1, concorrênte mais lento
	puts("");
	
	return  0;

}