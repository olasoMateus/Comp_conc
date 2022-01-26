class LE {
    private int leitor, escritor; //Mantém o número de leitores e escritores ativos
    public int modificada; // Mantém o valor da variável observada por nós, váriavel compartilhada


    //Aqui, temos basicamente o monitor implementado em aula
    //A grande diferença são métodos com chamadas diferentes, que usei para diferencia leitores e escritores de um leitorEscritor
    LE(){
        this.leitor = 0;
        this.escritor = 0;
        this.modificada = 0;//Variável compartilhada
    }

    public synchronized void entraLeitor(int id){
        try{
            while(this.escritor > 0){
                System.out.println("Leitor " + id+ " travado!");
                wait();
            }
            this.leitor++;
            System.out.println("Leitor " + id + " lendo!");
        }
        catch(InterruptedException e) { }
    }

    public synchronized void entraLeitor(int id, boolean a){
        try{
            while(this.escritor > 0){
                System.out.println("LeitorEscritor " + id+ " travado!");
                wait();
            }
            this.leitor++;
            System.out.println("LeitorEscritor " + id + " lendo!");
        }
        catch(InterruptedException e) { }
    }

    public synchronized void saiLeitor(int id){
        this.leitor--;
        if(this.leitor == 0){
            this.notify();
        }
        System.out.println("Leitor " + id + " terminou!");
    }

    public synchronized void saiLeitor(int id, boolean a){
        this.leitor--;
        if(this.leitor == 0){
            this.notify();
        }
        System.out.println("LeitorEscritor " + id + " terminou!");
    }

    public synchronized void entraEscritor(int id){
        try{
            while(this.escritor > 0 || this.leitor > 0){
                System.out.println("Escritor " + id+ " travado!");
                wait();
            }
            this.escritor++;
            System.out.println("Escritor " + id + " escrevendo!");
        }
        catch(InterruptedException e) { }
    }

    public synchronized void entraEscritor(int id, boolean a){
        try{
            while(this.escritor > 0 || this.leitor > 0){
                System.out.println("LeitorEscritor " + id+ " travado!");
                wait();
            }
            this.escritor++;
            System.out.println("LeitorEscritor " + id + " escrevendo!");
        }
        catch(InterruptedException e) { }
    }


    public synchronized void saiEscritor(int id){
        this.escritor--;
        this.notifyAll();
        System.out.println("Escritor " + id + " terminou!");
    }

    public synchronized void saiEscritor(int id, boolean a){
        this.escritor--;
        this.notifyAll();
        System.out.println("LeitorEscritor " + id + " terminou!");
    }


}

class Leitor extends Thread{
    int id; //Id da thread
    LE monitor; //Monitor da Thread
    int delay; //Delay para atrapalhar

    Leitor(int id, LE monitor, int delay){
        this.id = id;
        this.monitor = monitor;
        this.delay = delay;
    }

    public void run(){
        try{
            for(;;){
                this.monitor.entraLeitor(this.id);
                System.out.println("O valor da variavel eh " + this.monitor.modificada); //Faz a leitura da variável compartilhada
                this.monitor.saiLeitor(this.id);
                sleep(this.delay);
            }
        }
        catch(InterruptedException e) { }
    }
}

class Escritor extends Thread{
    int id; //Id da thread
    LE monitor; //Monitor da Thread
    int delay; //Delay para atrapalhar

    Escritor(int id, LE monitor, int delay){
        this.id = id;
        this.monitor = monitor;
        this.delay = delay;
    }

    public void run(){
        try{
            for(;;){
                this.monitor.entraEscritor(this.id);
                this.monitor.modificada = this.id; //Modifica o valor da variável compartilhada
                this.monitor.saiEscritor(this.id);
                sleep(this.delay);
            }
        }
        catch(InterruptedException e) { }
    }
}

class LeitorEscritor extends Thread{
    int id; //Id da thread
    LE monitor; //Monitor da Thread
    int delay; //Delay para atrapalhar

    LeitorEscritor(int id, LE monitor, int delay){
        this.id = id;
        this.monitor = monitor;
        this.delay = delay;
    }

    public void run(){
        try{
            for(;;){
                this.monitor.entraLeitor(this.id, true);
                System.out.println("O valor da variavel eh " + this.monitor.modificada); //Le o valor da variável comparilhada
                this.monitor.saiLeitor(this.id, true);
                sleep(this.delay);
                this.monitor.entraEscritor(this.id, true);
                this.monitor.modificada = this.monitor.modificada + 1; //Modifica o valor da variável comparilhada
                this.monitor.saiEscritor(this.id, true);
                sleep(this.delay);
            }
        }
        catch(InterruptedException e) { }
    }
}

class execucaoLeitorEscritor{
    static final int L = 4;
    static final int E = 4;
    static final int LE = 4;

    public static void main(String[] args){
        LE monitor = new LE();
        Leitor[] l = new Leitor[L];
        Escritor[] e = new Escritor[E];
        LeitorEscritor[] le = new LeitorEscritor[LE];


        //Cria e inicializa as threads de leitura, escrita e leituraEscrita
        for(int i = 0; i <  L ; i++){
            l[i] = new Leitor(i, monitor, (i + 1)*500);
            l[i].start();
        }

        for(int i = 0; i <  E ; i++){
            e[i] = new Escritor(i, monitor, (i + 1)*500);
            e[i].start();
        }

        for(int i = 0; i <  LE ; i++){
            le[i] = new LeitorEscritor(i, monitor, (i + 1)*500);
            le[i].start();
        }
    }
}