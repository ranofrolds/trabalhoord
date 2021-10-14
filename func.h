#ifndef FUNC
#define FUNC

int leia_campo(char campo[], int tamanho, FILE *entrada);
int concatenacao_buffer(char buffer[64], FILE *entrada);
int input(char str[], int size);
int executa_operacoes(char *file_name);
void imprime_ped();
void importacao_arq(char *filename);


#endif