import java.util.Arrays;

class Vet {
	//estrutura de dados
	private int[] v;
	
	public Vet(int N) {
		this.v = new int[N];
		for(int i = 0; i<N;i+=1){
			v[i] = i;
		}
	}
	
	public int get(int idx){
		return v[idx];
	}
	
	public int getLength(){
		return v.length;
	}
	
	public void change(int idx, int val){
		v[idx] = val;
	}
	
	public void printa(){
		System.out.println(Arrays.toString(v));
	}
}

//classe que estende Thread e implementa a tarefa de cada thread do programa 
class T extends Thread {
   //identificador da thread
   private int id;
   //objeto compartilhado com outras threads
   private Vet A;
   private Vet B;
   private Vet C;
   //numero de threads
   private int Nthreads;
   
   //construtor
   public T(int tid, Vet a, Vet b, Vet c, int nt) { 
      this.id = tid; 
      this.A = a;
      this.B = b;
      this.C = c;
      this.Nthreads = nt;
   }

   //tarefa da thread
   public void run() {
      for (int i=id; i<A.getLength(); i+=Nthreads) {
         C.change(i,A.get(i)+B.get(i));
      }
      System.out.println("Thread " + this.id + " terminou!"); 
   }
}

//classe da aplicacao
class Lab6 {
   //numero de threads
   static final int Nthreads = 4;
   //numero de elementos dos vetores
   static final int N = 8;

   public static void main (String[] args) {
      //reserva espaço para um vetor de threads
      Thread[] threads = new Thread[Nthreads];

      //cria uma instancia de cada recurso compartilhado entre as threads
      Vet A = new Vet(N);
      Vet B = new Vet(N);
      Vet C = new Vet(N);
	  
	  System.out.print("Valor de A = ");
	  A.printa();
	  System.out.print("Valor de B = ");
	  B.printa();
	  
	  
      //cria as threads da aplicacao
      for (int i=0; i<threads.length; i++) {
         threads[i] = new T(i, A, B, C, Nthreads);
      }

      //inicia as threads
      for (int i=0; i<threads.length; i++) {
         threads[i].start();
      }

      //espera pelo termino de todas as threads
      for (int i=0; i<threads.length; i++) {
         try { threads[i].join(); } catch (InterruptedException e) { return; }
      }

      System.out.print("Valor de C = ");
      C.printa(); 
   }
}
