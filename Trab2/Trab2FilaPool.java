import java.util.*;

//para conseguir inteiros aleatorios
import java.util.concurrent.ThreadLocalRandom;



//Criada apenas para poder retornar 2 inteiros em uma funcao
class Bounds {
	private int in;
	private int fin;
	
	public Bounds(int in,int fin){
		this.in = in;
		this.fin = fin;
	}
	
	public int getIn(){
		return this.in;
	}
	
	public int getFin(){
		return this.fin;
	}
	
}

//Threads
class QSThread extends Thread{

	private QSortConc qs;
	private int id;
	
	private Bounds bounds; //trecho a ser separado
	private int j;
	
	public QSThread(QSortConc qs, int id){
		this.qs = qs;
		this.id = id; //para prints
		//this.bound = qs.nullBounds;
		this.j = 0;
	}
	
	//tarefa da thread
	public void run(){
		System.out.println("Thread " + id + " comecou");
		while(true){
			bounds = qs.getTask();
			if(bounds==qs.nullBounds) break;
			//System.out.println("Thread " + id + " comecou " + bounds.getIn() + " - " + bounds.getFin());
			j=qs.separa(bounds.getIn(),bounds.getFin());
			//System.out.println("Thread " + id + " terminou " + bounds.getIn() + " - " + bounds.getFin());
			
			qs.endTask(bounds.getIn(),bounds.getFin(),j);
			
		}
	}
}
//monitor
class QSortConc{
	LinkedList<Bounds> q; //fila 
	int count;
	int tam;
	int[] v;
	int nthreads;
	Thread[] t;
	public Bounds nullBounds; //Variavel boba para nao retornar null
	
	public QSortConc(int[] v, int nthreads){
		this.q = new LinkedList<Bounds>();
		this.v = v;
		this.count = 0;
		this.tam = v.length;
		this.nthreads=nthreads;
		t = new Thread[nthreads];
		//inicializa fila;
		this.q.addFirst(new Bounds(0,this.tam-1));
		
		this.nullBounds = new Bounds(-1,-1); //coordenadas negativas
	}
	
	public synchronized void printV(){
		for(int i=0;i<tam;i++) System.out.print(v[i]+" ");
		System.out.println("");
	}
	
	//Funcao para thread tentar pegar tarefa nova
	//se acabaram as tarefas, retorna (-1,-1)
	public synchronized Bounds getTask(){
		while(q.isEmpty()){
			if(count == tam) {
				notifyAll(); //Para ter certeza de fim das threads
				return nullBounds;
			}
			try{wait();} catch (InterruptedException e) { System.out.println("ERRO de wait"); return nullBounds; }
		}
		Bounds ret = q.getLast();
		q.removeLast();
		return ret;
	}
	
	//Fim da tarefa da thread
	//retorna falso para terminar a thread
	//retorna true para continuar
	public synchronized void endTask(int p,int r, int j){
		if(j==-1) count++; //caso de ser vetor de tam = 1
		else if(j!=-2){ //caso nao seja um vetor vazio
			count++;
			q.addFirst(new Bounds(p,j-1));
			q.addFirst(new Bounds(j+1,r));
		}
		notifyAll();
	}
	
	//retorna o novo indice do pivot
	//codigo retirado do livro (da internet, para ser honesto)
	//"Introduction to Algorithms" de Cormen, Leiserson, Rivest e Stein
	public int separa(int p,int r){
		if(p==r) return -1; // nao separar
		if(p>r) return -2; // nao separar
		int c = v[r]; // pivot
		int t, j = p;
		for (int k = p; k < r; ++k)
			try{
				if (v[k] <= c) {
				t = v[j]; v[j] = v[k]; v[k] = t; //swap(v[k],v[j])
				j++; 
			} 
			} catch (Exception e) {System.out.println("Erro em "+p+" e "+r );}
		t = v[j]; v[j] = v[r]; v[r] = t; //swap(v[j],v[r])
		return j; 
	}
	
	public void sort(){
		for (int i=0; i<nthreads; i++) {
			t[i] = new QSThread(this,i+1);
			t[i].start();
		}
		for (int i=0; i<nthreads; i++) {
			try { t[i].join(); } catch (InterruptedException e) { return; }
		}
	}
	
}

class Trab2FilaPool{
	static final int nthreads = 1; //Testar 1, 2 e 4.
	
	//Nao levo em conta um vetor vazio ou de tamanho 1, vai dar erro
	static final int tam = 200000; //tamanho do vetor
	
	public static void main (String[] args) {
		//Cria vetor
		int[] v = new int[tam];
		
		//TESTES
		
		//Teste de numero aleatórios
		for(int i=0; i<tam; i++) {
			v[i] = ThreadLocalRandom.current().nextInt(-tam, tam + 1);
		}
		
		//Teste de n^2
		/*
		for(int i=0; i<tam; i++){
			v[i] = tam-i;
		}
		*/
		

		//Cria QSortConc
		QSortConc qs = new QSortConc(v,nthreads);
		//qs.printV(); //print do vetor
		
		//Inicia timer
		long startTime = System.nanoTime();

		//Faz o sort
		qs.sort();

		//Fim timer
		long endTime = System.nanoTime();
		long duration = (endTime - startTime);
		System.out.println(duration/1000000 +" milisegundo(s)");
		
		
		//qs.printV(); //print do vetor
		for(int i=1; i<tam; i++){
			if(v[i] < v[i-1]){
				System.out.println("Erro no teste de corretude da resposta!"); 
				break;
			}
		}
	}
}
