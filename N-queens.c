//bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//parametros globais
#define tam_pop 5
#define tam_genes 8
#define elitismo 0.6
#define prob_mut 0.1
#define prob_cruz 0.9

	
int *gerarSoluc(){
	static int *soluc;
	unsigned int genes, j, ponto1, ponto2, aux;
	soluc = (int*) calloc(tam_genes, sizeof(int));
	
	for(genes=0; genes < tam_genes; genes++)
	{
		soluc[genes] = genes;
	}
	
	for(j=0; j<tam_genes-1; j++)
	{
		ponto1 = rand() % tam_genes;
		ponto2 = rand() % tam_genes;
			
		aux = soluc[ponto1];
		soluc[ponto1] = soluc[ponto2];
		soluc[ponto2] = aux;
	}

	return soluc;
	 
}	
	
int fitness(int *individuo)
{
	int i, j, score = (tam_genes * (tam_genes - 2)) + 1;
	
	for(i=0; i<tam_genes-1; i++)
	{
		for(j=0; j<tam_genes; j++)
		{
			if(individuo[i] == individuo[j] && i != j)
			{
				score -= 1;
			}
			
			if((i+individuo[i] == j+individuo[j] && i != j) || (individuo[i] - i == individuo[j] - j && i != j))
			{
				score -= 1;
			}
			
			if(individuo[i] == individuo[i+1]+1)
			{
				score -= 1;
			}
		}
		
		
	}
	return score;
}

void crossOver(int i1, int i2, int **populacao, int *filho){
	
	int pos = rand() % tam_genes;
	int i;
	
	for(i=0; i<=pos; i++){
		filho[i] = populacao[i1][i];
	}
	
	for(i=pos+1; i<tam_genes; i++){
		filho[i] = populacao[i2][i];
	}
}


void mutacao(int *individuo)
{
	int ponto = rand() % tam_genes, atual;
	int ponto2, temp;
	do{
		ponto2 = rand() % tam_genes;
	}
	while(ponto == ponto2);
	
	temp = individuo[ponto];
	individuo[ponto] = individuo[ponto2];
	individuo[ponto2] = temp;
	
	
}

int selecao(int ** populacao){

	int soma = 0;
	int i;
	int selec;
	for(i=0; i<tam_pop; i++)
		soma += fitness(populacao[i]);
	int n = rand() % soma;
	soma = 0;
	
	for(i=0; i<tam_pop;i++){
		soma += fitness(populacao[i]);
		if (soma > n){
			selec = i;
			break;
		} 
	}
	return selec;
}

int obterMelhor(int **populacao)
{
	int i, scoreMelhor = fitness(populacao[0]), indiceMelhor = 0;
	
	for(i=1; i<tam_pop; i++)
	{
		int score = fitness(populacao[i]);
		if(score > scoreMelhor)
		{
			indiceMelhor = i;
			scoreMelhor = score;
			
		}
	}
	return indiceMelhor;
}

void rankPop(int **populacao, int **elite)
{
	int i, j;
	int *aux;
	int **populacao_ordenada;
	populacao_ordenada = (int**) calloc(tam_pop, sizeof(int*));
	
	for(i=0; i<tam_pop;i++)
	{
		populacao_ordenada[i] = (int*) calloc(tam_genes, sizeof(int));
	}
	
	for(i=0; i<tam_pop;i++)
	{
		for(j=0; j<tam_genes;j++)
		{
			populacao_ordenada[i][j] = populacao[i][j];
		}
	}

	for(i=0; i<tam_pop-1; i++)
	{
		for(j=0; j<tam_pop-1-i; j++)
		{
			if(fitness(populacao_ordenada[j]) < fitness(populacao_ordenada[j+1]))
			{
				aux = populacao_ordenada[j+1];
				populacao_ordenada[j+1] = populacao_ordenada[j];
				populacao_ordenada[j] = aux;
			}
		}
	}
	
	for(i=0; i<((double) (elitismo*tam_pop)); i++)
	{
		for(j=0; j<tam_genes; j++)
		{
			elite[i][j] = populacao_ordenada[i][j];
		}
	}
	
}

double mediaFitness(int **populacao){
	int i, totalFit = 0;
	
	for(i=0; i<tam_pop;i++){
	    totalFit += fitness(populacao[i]);
	}
	return (double) (totalFit/tam_pop);
	
}

int main(){
	time_t t;
	srand(time(&t));
	
	int ** populacao, *filho, **elite, *individuo; // Matrizes ponteiros
	int i, j, k, selecao_a, selecao_b; // variaveis auxiliares
	int tam_elite = (int) (elitismo * tam_pop); // variavel que determina o tamanho da elite
	populacao = (int**) malloc(tam_pop * sizeof(int*)); // alocacao da matriz populacao
	filho = (int*) malloc(tam_genes * sizeof(int)); // alocacao da matriz filho
	elite = (int**) malloc(tam_elite * sizeof(int*)); // elite
	individuo = (int*) malloc(tam_genes * sizeof(int)); // individuo
	for(i=0; i<tam_pop;i++)
		populacao[i] = (int*) malloc(tam_genes * sizeof(int));
	
	for(i=0 ;i<tam_elite; i++){
		elite[i] = (int*) malloc(tam_genes * sizeof(int));
	}
	
	
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	
	for(i=0; i<tam_pop; i++){
		populacao[i] = gerarSoluc();
	}
	rankPop(populacao, elite);
	int geracao;	
	for(geracao = 0; ; geracao++){
		
		for(i=0; i<tam_pop;i++)
			populacao[i] = gerarSoluc();
		
		
		for(i=0; i<tam_elite; i++){
			if(fitness(populacao[i]) < fitness(elite[i])){
				populacao[i] = elite[i];
			}
		}			
		
		double prob = rand();
		int cruzamentos = (int) (tam_pop/3);
		
		for(i=0; i<cruzamentos; i++){
			int i1 = selecao(populacao);
			int i2 = selecao(populacao);
			crossOver(i1, i2, populacao, filho);
			prob = rand() ;
			
			if(prob < prob_mut)
				mutacao(filho);
			
			if(fitness(filho) > fitness(populacao[i1]))
				populacao[i1] = filho;
		}
		
		printf("\n\nGen %d", geracao + 1);
		printf("\nAverage fitness : %.2f", mediaFitness(populacao));
		printf("\n\nFittest ");
		for(i=0; i<tam_genes; i++)
			printf("%d ", populacao[obterMelhor(populacao)][i]);
		printf("\nScore: %d\n\n--------------------------------", fitness(populacao[obterMelhor(populacao)]));
		
		rankPop(populacao, elite);
		if(fitness(populacao[obterMelhor(populacao)]) == ((tam_genes * (tam_genes - 2)) + 1)) break;
		
	}


	free(populacao);
	free(filho);
	free(elite);
	free(individuo);
	
	
	return 0;
}
