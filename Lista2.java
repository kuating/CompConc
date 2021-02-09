//Classe de Cópia de Arquivo a Imprimir--------------------------------
class ArqI{
	private String string;
	private int tid;
	private int num;
	private int impresso;
	
	public ArqI(int TID, int NUM){
		this.impresso = 0;
		this.tid = TID;
		this.num = NUM;
		
		//gerar conteudo da string
		this.string =
		String.format("Arquivo da Maquina: " + tid + " Numero: " + num);
	}
	
	public void imprimiu(){
		impresso = 1;
	}
	
	public int seImprimiu(){
		return impresso;
	}
	
	@Override
	public String toString() {
		return this.string;
	}
}

//Impressora-----------------------------------------------------------
class Impressora extends Thread {
	
	private int N = 10; //Limite de tamanho da fila de requisicoes
	private ArqI[] buffer; //Fila de requisicoes
	private int count;
	private int inicio;
	private int fim;
	
	private int imprimindo;
	
	public Impressora(){
		this.buffer = new ArqI[N];
		this.count = 0;
		this.inicio = 0;
		this.fim = 0;
		this.imprimindo = 0;
	}
	
	//Ignore a função comentada abaixo por favor
	
	/*public synchronized void printLista(){
	System.out.println("Lista Atual: (inicio ate final da fila)");
		if(inicio<fim){
			for(int i = inicio; i<=fim; i++){
				System.out.println(buffer[i].toString());
			}
		}
		if(inicio>fim){
			for(int i = inicio; i<N; i++){
				System.out.println(buffer[i].toString());
			}
			for(int i = 0; i<=fim; i++){
				System.out.println(buffer[i].toString());
			}
		}
	}*/
	
	public synchronized void waitForImp(ArqI arq){
		while(arq.seImprimiu()==0){
			try {wait();} catch(InterruptedException e){}
		}
	}
	
	public synchronized void deposit(ArqI arq){
		while(count == N){
			try {wait();} catch(InterruptedException e){}
		}
		buffer[fim] = arq;
		fim=(fim+1)%N;
		count+=1;
		notifyAll();
	}
	
	public void run() {
		while(true){
			//Aguardar por novas requisicoes de impressao
			synchronized (this) {
				while(count == 0 || imprimindo == 1) {
					try {wait();} catch(InterruptedException e){}
				}
				imprimindo=1;
			}
			//Imprimir uma das requicoes pendentes na ordem de recepcao
			
			System.out.println("Imprimindo...");
			//Processo de impressao
			try {Thread.sleep(400);} catch(InterruptedException e){}
			System.out.println(buffer[inicio].toString());
			
			synchronized(this){
				buffer[inicio].imprimiu();
				buffer[inicio] = null;
				inicio=(inicio+1)%N;
				count-=1;
				imprimindo=0;
				notifyAll();
			}
		}
	}
}

//Maquina do Usuario---------------------------------------------------
class Maquina extends Thread {

	int tid;
	Impressora impressora;
	
	public Maquina(int TID, Impressora imp){
		this.tid = TID;
		this.impressora = imp;
	}
	
	public void run() {
		int i = 0;
		while(true){
			ArqI arq = new ArqI(tid, i);
			impressora.deposit(arq);
			//Espera um pouco para pedir outra impressao
			impressora.waitForImp(arq);
			i+=1;
		}
	}
}

//Main
class Lista2 {
	
	static final int Nthreads = 4;
	
	public static void main (String[] args) {
		Impressora imp = new Impressora();
		
		//reserva espaço para um vetor de maquinas
		Thread[] threads = new Thread[Nthreads];
		
		
		//Inicializa Threads
		for (int i=0; i<threads.length; i++) {
			threads[i] = new Maquina(i+1,imp);
		}
		
		imp.start();
		
		//Inicia threads
		for (int i=0; i<threads.length; i++) {
			threads[i].start();
		}
		
		//Loop infinito
		while(true){
			
		}
	}
}
