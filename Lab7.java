//Classe que usaremos como Thread
class T extends Thread {
    //Identificador da thread, para auxiliar com a soma
    private int id;
    //Variável que contém guarda a soma dos elementos contados na Thread
    private int soma[];
    //Número de Threads, necessário para ordenar as posições que serão contadas pela Thread
    private int numDeThreads; 
    //Vetor que será somado;
    private int[] vetorDeInt;
  
    //Construtor
    public T(int tid, int soma[], int numDeThreads, int vetorDeInt[]) { 
        this.id = tid; 
        this.soma = soma;
        this.numDeThreads = numDeThreads;
        this.vetorDeInt = vetorDeInt;
    }

    //Método main da thread
    public void run() {
        System.out.println("Thread " + this.id + " iniciou!");
        this.soma[id] = 0;
        for (int i=this.id; i< vetorDeInt.length; i = i + this.numDeThreads) {
            this.soma[id] = this.soma[id] + this.vetorDeInt[i];

        }
        System.out.println("Thread " + this.id + " terminou!"); 
    }
}

//Classe da aplicacao
class SomaDeVetor {
    //Número de Threads
    static final int NThreads = 50;
    //Tamanho do vetor
    static final int Tam = 5000;

    public static void main (String[] args) {
        //Reserva espaço para um vetor de threads
        Thread[] threads = new Thread[NThreads];

        //Cria um vetor de inteiros
        int[] vetorDeInt = new int[Tam];

        //Cria um vetor que armazena as somas de cada Thread
        int[] somaDeThread = new int[NThreads];

        //Cria a variável da soma total
        int somaTotal = 0;

        //Inicializa o vetor de inteiros
        for (int i=0; i<Tam; i++) {
            vetorDeInt[i] = 1;
        }

        //Cria as threads da aplicacao
        for (int i=0; i<NThreads; i++) {
            threads[i] = new T(i, somaDeThread, NThreads, vetorDeInt);
        }

        //Inicia as threads
        for (int i=0; i<threads.length; i++) {
            threads[i].start();
        }

        //Espera pelo termino de todas as threads
        for (int i=0; i<threads.length; i++) {
            try { 
                threads[i].join();
                somaTotal = somaTotal + somaDeThread[i];
            }
            catch (InterruptedException e) { 
                return; 
            }
        }

        System.out.println("Valor da soma Total = " + somaTotal); 
   }
}
