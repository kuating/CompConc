#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(){
	//Cria arquivo
	
	srand(time(NULL));   // Inicializacao rand
	FILE *fp;
	
	int temp;
	
	fp = fopen( "arquivo.dat" , "wb" );
	
	long long int tam=2000000;
	
	fwrite(&tam, sizeof(long long int) , 1 , fp );
	
	for(long long int i = 0; i<tam; i++){
		temp = rand()%10;
		fwrite(&temp , sizeof(int) , 1 , fp );
	}
	fclose(fp);
	
	/////////////////////////////////
	
	/*fp = fopen( "arquivo.dat", "rb" );
	long long int result;
	
	fread(&result,sizeof(long long int),1,fp);
	printf("%lld\n",result);
	
	for(long long int i = 0; i<tam; i++){
		if(!fread(&temp , sizeof(int) , 1 , fp )) break;
		printf("%d ",temp);
	}
	printf("\n");
	
	fclose(fp);
	
	return(0);*/
}
