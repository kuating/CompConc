
// ThreadPool do proprio java
import java.text.SimpleDateFormat;  
import java.util.Date; 
import java.util.concurrent.ExecutorService; 
import java.util.concurrent.Executors; 
import java.util.concurrent.TimeUnit;
import java.util.concurrent.ThreadLocalRandom;

//monitor
class QuickSort{
	
	private int tam;
	private int count;
	private int[] v;
	private int nthreads;
	private ExecutorService pool;
	
	public QuickSort(int[] v,int nthreads){
		this.v=v;
		this.tam=v.length;
		this.nthreads=nthreads;
		this.count=0;
		this.pool = Executors.newFixedThreadPool(nthreads);
	}
	
	public synchronized void countUp(int plus){
		count+=plus;
		
		if(count==tam) pool.shutdownNow();
	}
	
	public void exec(Runnable task){
		pool.execute(task);
	}
	
	public void sort () {
		Runnable start = new Task(v,0,tam-1,this);
		pool.execute(start);
		try{ pool.awaitTermination(10, TimeUnit.SECONDS);}
		catch (InterruptedException e) { System.out.println("ERRO no \"join\""); }
	}
}

// Task class to be executed (Step 1) 
class Task implements Runnable {
	private int[] v;
	private int tam;
	private int p;
	private int r;
	private QuickSort qs; //monitor
	
	public Task(int[] v,int p,int r,QuickSort qs){
		this.v=v;
		this.tam=v.length;
		this.p=p;
		this.r=r;
		this.qs = qs;
	}
	
	public int separa (int p, int r) {
		int c = v[r]; // pivot = ultima posicao
		int t; //temp
		int j = p;
		for (int k = p; k < r; ++k)
			if (v[k] <= c) {
				t = v[j]; v[j] = v[k]; v[k] = t;
				j++; 
			} 
		t = v[j]; v[j] = v[r]; v[r] = t;
		return j; 
	}
	
	public void run() {
		int count=0;
		int j = separa(p,r);
		if( (j-1) > p ){
			Runnable left = new Task(v,p,j-1,qs);
			qs.exec(left);
		} else if((j-1)==p) count++;
		if( r > (j+1) ){
			Runnable right = new Task(v,j+1,r,qs);
			qs.exec(right);
		} else if(r==(j+1)) count++;
		count++;
		//
		qs.countUp(count);
	}
}
public class Trab2JavaPool {
	 //Numero de threads na pool
	static final int nthreads = 1;
	static final int tam = 200000;
	
	public static void main(String[] args) {
		//Cria vetor
		int[] v = new int[tam];
		
		//TESTES
		
		//Teste de numero aleatórios
		/*for(int i=0; i<tam; i++) {
			v[i] = ThreadLocalRandom.current().nextInt(-tam, tam + 1);
		}
		*/
		//Teste de n^2
		
		for(int i=0; i<tam; i++){
			v[i] = tam-i;
		}
		
		
		long startTime = System.nanoTime();
		
		QuickSort qs = new QuickSort(v,	nthreads);
		qs.sort();
		
		long endTime = System.nanoTime();
		long duration = (endTime - startTime);
		System.out.println(duration/1000000 +" milisegundo(s)");
		
		//print do vetor
		//for(int i=0;i<tam;i++) System.out.print(v[i]+" "); System.out.println("");
		
		for(int i=1; i<tam; i++){
			if(v[i] < v[i-1]){
				System.out.println("Erro no teste de corretude da resposta!"); 
				break;
			}
		}
	}
}
