/*\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__

  AO PREENCHER ESSE CABEÇALHO COM O MEU NOME E O MEU NÚMERO USP, 
  DECLARO QUE SOU O ÚNICO AUTOR E RESPONSÁVEL POR ESSE PROGRAMA. 
  TODAS AS PARTES ORIGINAIS DESSE EXERCÍCIO PROGRAMA (EP) FORAM 
  DESENVOLVIDAS E IMPLEMENTADAS POR MIM SEGUINDO AS INSTRUÇÕES DESSE EP
  E QUE PORTANTO NÃO CONSTITUEM PLÁGIO. DECLARO TAMBÉM QUE SOU RESPONSÁVEL
  POR TODAS AS CÓPIAS DESSE PROGRAMA E QUE EU NÃO DISTRIBUI OU FACILITEI A
  SUA DISTRIBUIÇÃO. ESTOU CIENTE QUE OS CASOS DE PLÁGIO SÃO PUNIDOS COM 
  REPROVAÇÃO DIRETA NA DISCIPLINA.

  Nome: Matheus Barbosa Silva
  NUSP: 11221741

  turtledorm.c

  Referências: 

  - método chase the lights de:
  https://help.gnome.org/users/lightsoff/stable/strategy.html.en

  - função randomInteger() de: 

  http://www.ime.usp.br/~pf/algoritmos/aulas/random.html

  \__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*---------------------------------------------------------------*/
/* 
 * 0. C O N S T A N T E S 
 */

/* tamanho máximo de um turtledorm */
#define MAX      128

/* estado da turtle */
#define ACORDADO   '#'
#define DORMINDO   ' '
#define TAPINHA    'T'

#define TRUE       1
#define FALSE      0

#define ENTER      '\n'
#define FIM        '\0'
#define ESPACO     ' '

/*---------------------------------------------------------------*/
/*
 * 1. P R O T Ó T I P O S
 */

/* PARTE I */
void
leiaTurtledorm(int *nLin, int *nCol, int tDorm[][MAX]);

void 
mostreTurtledorm(int nLin, int nCol, int tDorm[][MAX], char c);

void
tapinhaTurtle(int nLin, int nCol, int tDorm[][MAX], int lin, int col);

int 
todosDormindo(int nLin, int nCol, int tDorm[][MAX]);

int
graveTurtledorm(int nLin, int nCol, int tDorm[][MAX]);

/* PARTE II */
void
sorteieTurtledorm(int *nLin, int *nCol, int tDorm[][MAX]);

/* PARTE III */

void
resolvaTurtledorm(int nLin, int nCol, int tDorm[][MAX]);

void
incremente(int bin[]);

/* FUNÇÕES AUXILIARES */
void
inverteCelula(int nLin, int nCol, int tDorm[][MAX], int lin, int col);

int 
randomInteger(int low, int high);

int
contaTurtlesDespertos(int nLin, int nCol, int tDorm[][MAX]);

/*---------------------------------------------------------------*/
/* 
 *  M A I N 
 */
int main(int argc, char *argv[]){
	char tipoDeCarregamento, jogada;
	int nLin, nCol, tDorm[MAX][MAX], jogar = 1, linJog, colJog, quantTapinhas = 0;

    printf("Light Out\n");
    printf("------------------------------------------------------------\n");

	printf("Digite 's' para (s)ortear ou\n");
    printf("       'l' para (l)er um turtledorm de arquivo.\n>>> ");
	scanf("%c", &tipoDeCarregamento);

	if(tipoDeCarregamento == 'l')
		leiaTurtledorm(&nLin, &nCol, tDorm);
	else
        sorteieTurtledorm(&nLin, &nCol, tDorm);
    
    printf("\nTurtledorm inicial\n");
    mostreTurtledorm(nLin, nCol, tDorm, ACORDADO);
    printf("Um tapinha é definido por dois inteiros lin e col, onde\n");
    printf("    1 <= lin <= %d e 1 <= col <= %d\n\n", nLin, nCol);
    printf("------------------------------------------------------------\n");

    while (!todosDormindo(nLin, nCol, tDorm) && jogar == 1){
        printf("Digite 'd' para (d)esistir,\n");
        printf("       'a' para receber (a)juda para encontrar uma solucao,\n");
        printf("       'g' para (g)ravar o turtledorm atual, ou\n");
        printf("       'lin col' para dar um tapinha na posicao [lin][col].\n>>> ");
        scanf(" %c", &jogada);

        if(jogada == 'd'){
            jogar = 0;
        }else if(jogada == 'a'){
            resolvaTurtledorm(nLin, nCol, tDorm);
        }else if(jogada == 'g'){
            if (graveTurtledorm(nLin, nCol, tDorm) == EXIT_FAILURE){
                printf("Desculpe! Houve um erro durante a gravação do arquivo.\n");
                printf("Verifique o nome do arquivo e tente novamente.\n\n");
            }else{
                printf("Arquivo gravado com sucesso.\n\n");
            }
        }else{
            /*uso a função atoi para converter o char recebido em um int */
            linJog = atoi(&jogada);
            scanf(" %c", &jogada);
            colJog = atoi(&jogada);

            tapinhaTurtle(nLin, nCol, tDorm, linJog, colJog);
            quantTapinhas++;
            printf("Turtledorm apos %d tapinhas.\n", quantTapinhas);
            mostreTurtledorm(nLin, nCol, tDorm, ACORDADO);
        }
    }

    if(todosDormindo(nLin, nCol, tDorm)){
        printf("Parabens, voce colocou todos para dormir apos %d tapinha(s)!\n", quantTapinhas);
        printf("Todos ja estao dormindo. Nao faca barulho!\n");
    }else{
        printf("Desta vez nao deu.\n");
        printf("Voce deu %d tapinha(s).\n", quantTapinhas);
        printf("Melhor sorte na proxima!\n");
    }

    return EXIT_SUCCESS;
}

/*---------------------------------------------------------------*/
/* 
 *  I M P L E M E N T A Ç Ã O   D A S   F U N Ç Õ E S   DA  
 *                        P A R T E   I 
 */

void leiaTurtledorm(int *nLin, int *nCol, int tDorm[][MAX]){
    int i, j;
    char nomeArq[80];
    FILE * arquivoDeEntrada;

	printf("Digite o nome do arquivo de onde carregar o turtledorm: ");
    scanf("%s", &nomeArq[0]);

	arquivoDeEntrada = fopen(nomeArq, "r");
    fscanf(arquivoDeEntrada, "%d %d", &(*nLin), &(*nCol));

    /*Lê todos os nlin*nCol valores binários do arquivo e insere-os na matriz tDorm */
    for(i = 0; i < *nLin; i++){
        for(j = 0; j < *nCol; j++){
            if (fscanf(arquivoDeEntrada, "%d", &(tDorm[i][j]) ) != 1) continue;
        }
    }
}

void mostreTurtledorm(int nLin, int nCol, int tDorm[][MAX], char c){
    int i, j;
    printf("     ");
    /*Imprime os índices das colunas */
    for(j = 1; j <= nCol; j++){
        printf("   %d  ", j);
    }
    printf("\n");

    for(i = 0; i <= nLin; i++){
        printf("     +");
        for(j = 0; j < nCol; j++){
            printf("-----+");
        }
        printf("\n");
        for(j = 0; j <= nCol && i < nLin; j++){
            if(j == 0){
                printf("   %d ", i+1);
            }
            printf("|");
            if(j < nCol){
                if(tDorm[i][j] == 1){
                    printf("  %c  ", c);
                }else{
                    printf("  %c  ", DORMINDO);
                }
            }
        }
        printf("\n");
    }
}

void tapinhaTurtle(int nLin, int nCol, int tDorm[][MAX], int lin, int col){
    /*Uso a função auxiliar inverteCelula para inverter os valores adjacentes da posicao da jogada */
    inverteCelula(nLin, nCol, tDorm, lin, col);
    inverteCelula(nLin, nCol, tDorm, lin+1, col);
    inverteCelula(nLin, nCol, tDorm, lin-1, col);
    inverteCelula(nLin, nCol, tDorm, lin, col+1);
    inverteCelula(nLin, nCol, tDorm, lin, col-1);
}

int todosDormindo(int nLin, int nCol, int tDorm[][MAX]){
    int i, j, todosDormindo = 1;
    /*inicio todosDormindo com 1 e se encontrar algum valor igual a 1 (acordado), retorno 0 */
    for(i = 0; i < nLin && todosDormindo == 1; i++){
        for(j = 0; j < nCol && todosDormindo == 1; j++){
            if (tDorm[i][j] == 1)
                todosDormindo = 0;
        }
    }
    return todosDormindo;
}

int graveTurtledorm(int nLin, int nCol, int tDorm[][MAX]){
    int i, j;
    char nomeArq[80];
    FILE * arquivoDeEntrada;

	printf("\nDigite o nome do arquivo onde deseja carregar seu turtledorm: ");
    scanf("%s", &nomeArq[0]);

    /*abertura do arquivo com o modo de escrita */
	arquivoDeEntrada = fopen(nomeArq, "w");
    fprintf(arquivoDeEntrada, "%d %d\n", nLin, nCol);

    for(i = 0; i < nLin; i++){
        for(j = 0; j < nCol - 1; j++){
            if (fprintf(arquivoDeEntrada, "%d ", tDorm[i][j]) == EOF){
                return EXIT_FAILURE;
            }
        }
        if (fprintf(arquivoDeEntrada, "%d", tDorm[i][j]) == EOF){
            /*impressão do ultimo numero, sem espaço no final*/
            return EXIT_FAILURE;
        }
        /*pulo para a proxima linha do arquivo */
        fprintf(arquivoDeEntrada, "\n");
    }
    fclose(arquivoDeEntrada);
    return EXIT_SUCCESS;
}

/*---------------------------------------------------------------*/
/* 
 *  I M P L E M E N T A Ç Ã O   D A S   F U N Ç Õ E S   DA  
 *                       P A R T E   II 
 */
void sorteieTurtledorm(int *nLin, int *nCol, int tDorm[][MAX]){
    int semente, i, j, quantTapinhas, linRandom, colRandom;
    char dificuldade;

    printf("Digite a dimensao do turtledorm (nLin nCol): ");
    scanf("%d", &(*nLin));
    scanf("%d", &(*nCol));
    printf("Digite um inteiro para o gerador de numeros aleatorio (semente): ");
    scanf("%d", &semente);
    printf("Digite o nivel de dificuldade [f/m/d]: ");
    scanf(" %c", &dificuldade);
    
    /*inicializo tDorm com zeros*/
    for(i = 0; i < *nLin; i++){
        for(j = 0; j < *nCol; j++){
            tDorm[i][j] = 0;
        }
    }
    /* inicializa o gerador */
    srand(semente);

    /*determino a quantidade de tapinhas a partir de um número aleatório
    no intervalo (porcentagem) determinado no enunciado */    
    if(dificuldade == 'f'){
        quantTapinhas = randomInteger(0.05 * (*nLin) * (*nCol), 0.20* (*nLin) * (*nCol));
    }else if(dificuldade == 'm'){
        quantTapinhas = randomInteger(0.25 * (*nLin) * (*nCol), 0.50* (*nLin) * (*nCol));
    }else{
        quantTapinhas = randomInteger(0.55 * (*nLin) * (*nCol), 0.85* (*nLin) * (*nCol));
    }

    /*aciono a função tapinhaTurtle em uma posição sorteada */
    for(i = 0; i < quantTapinhas; i++){
        linRandom = randomInteger(1, (*nLin));
        colRandom = randomInteger(1, (*nCol));
        tapinhaTurtle(*nLin, *nCol, tDorm, linRandom, colRandom);
    }

    printf("Numero de tapinhas sorteado = %d.\n", quantTapinhas);
    printf("Numero de turtles despertos = %d.\n", contaTurtlesDespertos(*nLin, *nCol, tDorm));
}


/*---------------------------------------------------------------*/
/* 
 *  I M P L E M E N T A Ç Ã O   D A S   F U N Ç Õ E S   DA  
 *                       P A R T E   III 
 */

int chaseTheLights(int nLin, int nCol, int tDorm[][MAX], int tapinhas[][MAX]){
    int i, j, quantTapinha = 0;

    /*pelo método chase the lights, percorro toda a matriz, partindo da 2ª linha,
    aplicando tapinhas nas posições onde na mesma coluna e na linha superior há um
    turtle desperto. Assim, se no final todos os turtles estão dormindo, esta é uma
    solução válida. (Os tapas na primeira linha são coordenados na função resolvaTurtledorm) */
    for(i = 1; i < nLin ; i++){
        for (j = 0; j < nCol; j++){
            if(tDorm[i-1][j] == 1){
                tapinhas[i][j] = 1;
                tapinhaTurtle(nLin, nCol, tDorm, i+1, j+1);
                quantTapinha++;
            }else{
                tapinhas[i][j] = 0;
            }
        }
    }
    return quantTapinha;
}

void incremente(int bin[]){
    int i;
    /*percorre o vetor até encontrar um valor 0 (menos significativo), trocando-o para 1.
    O contrário é feito para os 1s menos significativos encontrados*/
    for (i = 0; bin[i] != 0; i++)
        bin[i] = 0;
    bin[i] = 1;
}

void resolvaTurtledorm(int nLin, int nCol, int tDorm[][MAX]){
    int bin[MAX*MAX+1];                                     /*cria o vetor para armazenar os numeros binarios */
    int tapinhas[MAX][MAX], menorTapinhas[MAX][MAX];        /* representacao da lista de tapinhas */
    int tDormCopia[MAX][MAX];                               /*cópia da matriz original para simular as jogadas */
    int i, j, contTapinhas = 0, menorContTapinhas = 0, quantLin = nLin;

    /* inicialize bin com zero
     * utilo o conteúdo de bin para testar todas as possibilidadades
     * de jogadas na primeira linha, completando o método chaseTheLights*/
    for (i = 0; i < nCol+1; i++){
        bin[i] = 0; 
    }

    while (bin[nCol] == 0){        
    	contTapinhas = 0;

        /*copie o conteúdo da matriz original para a matriz tDormCopia para as simulações */
    	for (i = 0; i < nLin; i++){
            for(j = 0; j < nCol; j++){
                tDormCopia[i][j] = tDorm[i][j];
            }
	    }
      
        /* transforme os 1s do vetor bin em tapinhas na matriz de simulação de jogada*/        
        for (i = 0; i < nCol; i++){
            tapinhas[0][i] = bin[i];
            if( bin[i] == 1 ){
                tapinhaTurtle(nLin, nCol, tDormCopia, 1, i+1);
                contTapinhas++;
            }
        }
        
        nLin = quantLin;
        /*aciono a função chaseTheLights para testar uma das possibilidades de jogo para o método */
        contTapinhas += chaseTheLights(nLin, nCol, tDormCopia, tapinhas);

        /* após o método ter sido aplicado, verifico se todos os turtles estão dormindo
         * e caso estejam e esta jogada seja a ótima entre as testadas até o momento,
         * armazeno-a para que esta seja mostrada ao usuário*/
        if( todosDormindo(nLin, nCol, tDormCopia) && (contTapinhas < menorContTapinhas || menorContTapinhas == 0) ){
        	menorContTapinhas = contTapinhas;
            for (i = 0; i < nLin; i++){
                for(j = 0; j < nCol; j++){
                    menorTapinhas[i][j] = tapinhas[i][j];
                }
            }
        }
        
        incremente(bin);
    }

    /* se menorContTapinhas == 0, esta variável nao foi alterada nenhuma vez
     * pois mantém-se com seu valor inicial, logo nenhuma solução foi encntrada*/
    if(menorContTapinhas == 0){
        printf("Nao tem solucao!\n");
    }else{
        mostreTurtledorm(quantLin, nCol, menorTapinhas, TAPINHA);
        printf("\nResolvido com %d tapinhas.\n", menorContTapinhas);
    }
}
/*---------------------------------------------------------------*/
/* 
 *  I M P L E M E N T A Ç Ã O   D A S   F U N Ç Õ E S   DA  
 *                     A U X I L I A R E S 
 */

void inverteCelula(int nLin, int nCol, int tDorm[][MAX], int lin, int col){
    if(lin <= nLin && col <= nCol){
        if (tDorm[lin-1][col-1] == 1){
            tDorm[lin-1][col-1] = 0;
        }else{
            tDorm[lin-1][col-1] = 1;
        }
    }
}

int contaTurtlesDespertos(int nLin, int nCol,int tDorm[][MAX]){
    int i, j, contDespertos = 0;
    for (i = 0; i < nLin; i++){
        for(j = 0; j < nCol; j++){
            if(tDorm[i][j] == 1) contDespertos++;
        }
    }
    return contDespertos;
}

/* 
 * randomInteger()
 *
 * A função recebe dois inteiros LOW e HIGH e retorna um 
 * inteiro aleatório entre LOW e HIGH inclusive, ou seja, 
 * no intervalo fechado LOW..HIGH.
 *
 * PrÃ©-condiÃ§Ã£o: a função supõe que 0 <= LOW <= HIGH < INT_MAX.
 *     O codigo foi copiado da página de projeto de algoritmos 
 *     de Paulo Feofiloff, que por sua vez diz ter copiado o 
 *     código da biblioteca random de Eric Roberts.
 */
int 
randomInteger(int low, int high)
{
    int k;
    double d;
    d = (double) rand( ) / ((double) RAND_MAX + 1);
    k = d * (high - low + 1);
    return low + k;
}