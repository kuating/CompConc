import java.util.Arrays;

class Buffer {
	//estrutura de dados
	private int[] buffer;
	private int tam;
	private int addPos; //(add position)
	private int remPos; //(remove position)
	private int count;
	
	
	//iniciar o buffer
	public Buffer(int N) {
		tam = N;
		this.buffer = new int[N];
		//inicializar com -1, indica posicao vazia
		for(int i = 0; i<N;i+=1){
			buffer[i] = -1;
		}
		//Posicao vazia a ser ocupada
		addPos = 0;
		count = 0;
	}
	
	//Imprimir elementos
	public void printa(){
		System.out.println(Arrays.toString(buffer));
	}
	
	//Inserir um elemento (soh em posicao vazia)
	public synchronized void insert(int tid) {
		try {
			while(count>=tam){
				System.out.printf("T %d: Inserção bloqueada (Buffer cheio)\n",tid);
				wait();
				System.out.printf("T %d: Inserção desbloqueada\n",tid);
			}
			buffer[addPos] = tid;
			count+=1;
			addPos = (addPos+1)%tam;
			System.out.printf("T %d: Inseriu elemento\n",tid);
			this.printa();
			notifyAll();
		}
		catch (InterruptedException e) { }
	}
	
	//Retirar um elemento (soh em posicao cheia, que se torna vazia)
	public synchronized void remove(int tid) {
		try {
			while(count<=0){
				System.out.printf("T %d: Remoção bloqueada (Buffer vazio)\n",tid);
				wait();
				System.out.printf("T %d: Remoção desbloqueada\n",tid);
			}
			//((addPos-1) mod tam)
			buffer[remPos] = -1;
			remPos = (remPos+1) % tam;
			count-=1;
			System.out.printf("T %d: Removeu elemento\n",tid);
			this.printa();
			notifyAll();
		}
		catch (InterruptedException e) { }
	}
}

//Threads//-------------------------------------------------------------
class Produtor extends Thread {
   //identificador da thread
   private int id;
   //objeto compartilhado com outras threads
   private Buffer buffer;
   //Tempo entre insercoes
   private int delay;
   
   //construtor
   public Produtor(int tid, Buffer b, int delay) { 
      this.id = tid; 
      this.buffer = b;
      this.delay = delay;
   }

   //tarefa da thread
	public void run() {
		while(true) {
			try{
				this.buffer.insert(id);
				sleep(delay); //espera
			}catch (InterruptedException e) { }	
		}
	}
}
class Consumidor extends Thread {
   //identificador da thread
   private int id;
   //objeto compartilhado com outras threads
   private Buffer buffer;
   //Tempo entre remocoes
   private int delay;
   
   //construtor
   public Consumidor(int tid, Buffer b, int delay) { 
      this.id = tid; 
      this.buffer = b;
      this.delay = delay;
   }

   //tarefa da thread
	public void run() {
		while(true) {
			try{
				this.buffer.remove(id);
				sleep(delay); //espera
			}catch (InterruptedException e) { }	
		}
	}
}
//----------------------------------------------------------------------

//classe da aplicacao
class Lab7 {
   //numero de produtores
   static final int P = 4;
   //numero de consumidores
   static final int C = 2;
   
   //tamanho do buffer
   static final int N = 8;
   
   //delay entre remocoes e insercoes
   static final int delay = 500;

   public static void main (String[] args) {
      //reserva espaço para um vetor de threads
      Thread[] threads = new Thread[P+C];

      //cria uma instancia de cada recurso compartilhado entre as threads
      Buffer buffer = new Buffer(N);
	  
	  System.out.print("Valor inicial do buffer = ");
	  buffer.printa();
	  
      //cria as threads da aplicacao
      for (int i=0; i<P; i++) {
         threads[i] = new Produtor(i,buffer,delay);
      }
      for (int i=P; i<P+C; i++) {
         threads[i] = new Consumidor(i,buffer,delay);
      }

      //inicia as threads
      for (int i=0; i<threads.length; i++) {
         threads[i].start();
      }
   }
}
