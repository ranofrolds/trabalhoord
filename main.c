#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//gcc main.c func.c -o programa


/*1. NÚMERO de inscrição do candidato (que é único e será usado como campo chave);
  2. NOME completo do candidato;
  3. CURSO no qual foi inscrito;
  4. SCORE final obtido pelo candidato.*/

/*•Importação dos dados;
  • Busca de registro pelo campo IDENTIFICADOR;
  • Inserção de novo registro;
  • Remoção de registro; 
  • Impressão da PED.*/

//tamanho fixo de 64 bytes com campos de tamanho de variável delimitados pelo caractere “|”

int main(int argc, char *argv[]) {
   
    if (argc == 3 && strcmp(argv[1], "-i") == 0) { // ./programa -i "arquivo.txt", importa um arquivo selecionado
        
        printf(ANSI_COLOR_GREEN "IMPORTACAO selecionado... \n" ANSI_COLOR_RESET "Nome do arquivo = " ANSI_COLOR_YELLOW "%s\n" ANSI_COLOR_RESET , argv[2]);
        importacao_arq(argv[2]);

    } else if (argc == 3 && strcmp(argv[1], "-e") == 0) { // ./programa -e "dados.dat", para executar operações em um arquivo

        printf(ANSI_COLOR_BLUE "EXECUCAO selecionado... \n\n" ANSI_COLOR_RESET "Nome do arquivo = "ANSI_COLOR_YELLOW"%s\n" ANSI_COLOR_RESET, argv[2]);
        executa_operacoes(argv[2]);
        

    } else if (argc == 2 && strcmp(argv[1], "-p") == 0) { // ./programa -p imprime a PED do arquivo.
 
        printf(ANSI_COLOR_MAGENTA "IMPRECAO selecionado ...\n\n"ANSI_COLOR_RESET);
        imprime_ped();

    } else {
        
        fprintf(stderr, "Argumentos incorretos!\n");
        fprintf(stderr, "Modo de uso:\n");
        fprintf(stderr, "$ %s (-i|-e) nome_arquivo\n", argv[0]);
        fprintf(stderr, "$ %s -p\n", argv[0]);
        exit(EXIT_FAILURE);

    }

    return 0;
}
//poma!!