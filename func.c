#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define TAM 64

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int input(char str[], int size){
    int i=0;
    char c=getchar();
    while(c!='\n'){
        if(i<size-1){
            str[i]=c;
            i++;
        }
        c=getchar();
    }
    str[i]='\0';
    return i;
}


int leia_campo(char *campo, int tamanho, FILE *entrada){
    int i = 0;
    char c;
    
    fread(&c, sizeof(char), 1, entrada);//pega a primeira letra da entrada para entrar no while

    while (!feof(entrada) && c != '|'){ //se o "c" não tiver no final do arquivo e nem estiver lendo uma pipe

        if(i < tamanho){//o delimitador máximo do campo é o tamanho do registro ou seja 64 bytes // quando o i for maior que tamanho ele para de escrever e espera chegar no IOF ou na Pipe.

            campo[i] = c; //isso vai fazer com que preencha o campo
            i++; 

        }

        fread(&c, sizeof(char), 1, entrada); //para confirmar se o arquivo acabou ou não, caso ele tenha chego na pipe ou EOF ele quebra o while 
    }
    
    campo[i] = '\0'; //colocar um indicador de que o campo terminou de ser lido

    // printf("%s", campo);
    return i; //quando ele terminar de ler os campos vai retorna o tanto de casas que foram usadas para escrever o campo, ou seja o tamanho do campo
}

int executa_operacoes(char *file_name){
    FILE *arq;
    char buffer[TAM+2]; //+2 por conta do 'x' e ' '
    char *chave, chave_reg[7], reg[TAM], *chave_aux, chave2[TAM];
    int campo=0;
    int i=0, j=0, z=0;
    char aux[4], aux2;
    int byteoffset=4;
    int rrn, new_rrn=0;
    int comp;
    bool achou=false;
    int contPipe = 0;

    printf("Digite a operacao a ser executada\n1. \""ANSI_COLOR_YELLOW"b"ANSI_COLOR_RESET" CHAVE\"\n2. \""ANSI_COLOR_YELLOW"i"ANSI_COLOR_RESET" REGISTRO\"\n3. \""ANSI_COLOR_YELLOW"r"ANSI_COLOR_RESET" CHAVE\"\n");
    printf("Digite aqui: ");
    input(buffer, 66);//operacao a ser executada junto a chave ou registro

    switch(buffer[0]){
        case 'b':
            chave=strtok(buffer," ");
            chave=strtok(NULL, "\0"); //separa a operacao da chave
            printf(ANSI_COLOR_GREEN"Busca pelo registro de chave"ANSI_COLOR_CYAN" \"%s\""ANSI_COLOR_RESET, chave);

            arq=fopen(file_name, "r");//abre o arq p leitura

            fseek(arq,byteoffset, SEEK_SET);//pula o cabecalho
            campo= leia_campo(chave_reg, 6, arq);//le o primeiro campo

            while(!achou && campo>0){//enquanto nao achou e ainda exista campo continua o loop
                if(strcmp(chave, chave_reg)==0){ //compara a chave buscada com a desejada
                    fseek(arq,byteoffset, SEEK_SET);//posiciona no comeco do registro para leitura completa
                    fread(reg, sizeof(char), TAM, arq);
                    achou=true;

                    printf("\n");
                    for(z=0; z<TAM; z++){
                        printf("%c",reg[z]);
                        if(reg[z]== '|'){
                            contPipe++;
                            if(contPipe == 4){
                                printf(ANSI_COLOR_MAGENTA"\nRRN ->"ANSI_COLOR_YELLOW" %d"ANSI_COLOR_RESET" e "ANSI_COLOR_MAGENTA"ByteOffset -> "ANSI_COLOR_YELLOW"%d"ANSI_COLOR_RESET, i, byteoffset);
                                return EXIT_SUCCESS;
                            }
                        }
                    }
                    fclose(arq);
                    return EXIT_SUCCESS;
                }
                else{
                    i++;
                    byteoffset=i*TAM+sizeof(int);
                    fseek(arq,byteoffset, SEEK_SET);//proximo registro
                    campo=leia_campo(chave_reg,6, arq);//le campo
                }   
            }
            printf(ANSI_COLOR_RED"\nRegistro nao encontrado!!"ANSI_COLOR_RESET);
            fclose(arq);
            return EXIT_FAILURE;

            break;
        case 'i':
            chave=strtok(buffer," "); //remover o i
            chave=strtok(NULL, "\0"); //armazena a chave

            arq=fopen(file_name, "r+");

            fread(&rrn, sizeof(int),1,arq); //le o cabeçalho

            campo= leia_campo(chave_reg, 6, arq);//le o primeiro campo

            for(i=0; i < strlen(chave);i++){
                chave2[i] = toupper(chave[i]);
            }

            strcpy(chave, chave2);

            strcpy(reg, chave);

            chave_aux=strtok(reg,"|"); //remover o i

            while(!achou && campo>0){//enquanto nao achou e ainda exista campo continua o loop
                if(strcmp(chave_aux, chave_reg)==0){ //compara a chave buscada com a desejada
                    achou=true;
                    printf(ANSI_COLOR_RED"O registro ja existe no arquivo!\n"ANSI_COLOR_RESET);
                    fclose(arq);
                    return EXIT_FAILURE;
                }
                else{
                    i++;
                    byteoffset=i*TAM+sizeof(int);
                    fseek(arq,byteoffset, SEEK_SET);//proximo registro
                    campo=leia_campo(chave_reg,6, arq);//le campo
                }   
            }
           
            if(rrn==-1){
                if(strlen(chave)<64){
                    comp=strlen(chave);
                    for(i=comp; i<TAM; i++){
                        strcat(chave, ".");//preenche o resto do registro com .
                    }
                    chave[64] = '\0';
                }
                fseek(arq, 0,SEEK_END);//vai para o final do arquivo
                fputs(chave, arq);//insere ele 
                printf(ANSI_COLOR_GREEN"\nArquivo Inserido!\n"ANSI_COLOR_RESET);
                fclose(arq);
                return EXIT_SUCCESS;
            }
            else{
                fseek(arq, rrn*TAM+sizeof(int), SEEK_SET);//encontra o registro
                fread(&aux2,sizeof(char), 1,arq);//ver se é *
                
                if(aux2=='*'){ //se for *
                   fseek(arq, rrn*TAM+sizeof(int)+2*sizeof(char), SEEK_SET);
                
                // vai ler o rrn do *|2|, a partir do numero em si, ate encontrar a ultima pipe
                   aux[j]=fgetc(arq);
                   while(aux[j]!='|'){
                        j++;
                        aux[j]=fgetc(arq);
                   }
                   new_rrn=atoi(aux); //converte string para int
                   
                   comp=strlen(chave);
                   if(comp<TAM){
                       for(i=comp; i<TAM; i++){
                           strcat(chave, ".");//preenche com . ate o final do registro
                       }
                       chave[64] = '\0';
                   }

                   fseek(arq, rrn*TAM+sizeof(int), SEEK_SET);
                   fputs(chave, arq);
                   printf(ANSI_COLOR_GREEN"Arquivo Inserido!\n"ANSI_COLOR_RESET);
                   fseek(arq, 0, SEEK_SET);
                   fwrite(&new_rrn,sizeof(int), 1,arq);
                   fclose(arq);
                   return EXIT_SUCCESS;
                }
            }

            fprintf(stderr,ANSI_COLOR_RED"Ocorreu algum erro!!"ANSI_COLOR_RESET);
            fclose(arq);
            return EXIT_FAILURE;

            break;
        case 'r':
            chave=strtok(buffer," ");
            chave=strtok(NULL, "\0");

            arq=fopen(file_name, "r+");

            fread(&rrn,sizeof(char), 4,arq);
            campo= leia_campo(chave_reg, 6, arq);//le o primeiro campo
            
            while(!achou && campo>0){ //enquanto nao achar o registro e ainda existir registros continua o loop
                if(strcmp(chave, chave_reg)==0){ //compara a chave buscada com a desejada

                    fseek(arq,byteoffset, SEEK_SET);//posiciona no comeco do registro para leitura completa
                    sprintf(aux, "*|%d|", rrn); //esse rrn é o do cabeçalho no topo da PED
                    fputs(aux, arq); //aqui o topo da PED entra no começo do resgistro
                    achou=true;
                    printf(ANSI_COLOR_GREEN"\nArquivo Removido!!\n"ANSI_COLOR_RESET);
                    fseek(arq, 0, SEEK_SET); //vai no começo do arquivo, vulgo topo da PED
                    fwrite(&new_rrn,sizeof(int), 1,arq); //coloca o RRN do registro que foi removido
                    fclose(arq);
                    return EXIT_SUCCESS;
                }
                else{ //caso ele não encontra a chave 
                    new_rrn++;//vai para o próximo RRN
                    byteoffset=new_rrn*TAM+sizeof(int);
                    fseek(arq,byteoffset, SEEK_SET);//proximo registro
                    campo=leia_campo(chave_reg,6, arq);//le campo
                }//volta pro while
            }

            printf(ANSI_COLOR_RED"\nRegistro nao encontrado!!"ANSI_COLOR_RESET);
            fclose(arq);
            return EXIT_FAILURE;
            break;
        default:
            printf(ANSI_COLOR_RED"Comando nao reconhecido!... Encerrando"ANSI_COLOR_RESET);
            fclose(arq);
            return EXIT_FAILURE;
            break;
    }
}

int imprime_ped(){
    FILE *arq;
    char buffer[TAM], aux1[4];
    int free_space=0;
    int *rrn, new_rrn;
    int topo_ped;
    int found=0;
    char aux;
    int j=0;
    
    arq=fopen("dados.dat", "r"); //abre para leitura
    if(arq==NULL){
        fprintf(stderr, ANSI_COLOR_RED"Nao foi possivel ler o arquivo"ANSI_COLOR_RESET, "dados.dat");
        return EXIT_FAILURE;
    }
    
    fread(&topo_ped, sizeof(int),1,arq);//ler cabeçalho, topo da ped
    if(topo_ped == -1){
        printf("Nenhum registro foi removido!\n Espaco Disponivel: [0]");
        return EXIT_SUCCESS;
    }

    //aloca o primeiro rrn (o topo da ped)
    rrn=malloc(sizeof(int)); //aloca na memoria um espaço de tamanho int
    if(rrn==NULL){ //se não tiver espaço retorna NULL
        return EXIT_FAILURE;
    }
    rrn[0]=topo_ped;
    found++; 
    
    while(rrn[found-1]!= -1){
        j=0;
        memset(aux1,0,strlen(aux1)); //resetar os valores da string aux1
        fseek(arq, rrn[found-1]*TAM+sizeof(int), SEEK_SET);  //reposiciona o indicador para o proximo registro(rrn)
        fread(&aux, sizeof(char), 1, arq ); //le o primeiro byte, se for (*), entao o registro foi removido previamente
        if(aux=='*'){   
            fseek(arq, rrn[found-1]*TAM+sizeof(int)+2*sizeof(char), SEEK_SET);
            // vai ler o rrn do *|2|, a partir do numero em si, ate encontrar a ultima pipe
            aux1[j]=fgetc(arq);
            while(aux1[j]!='|'){ //pra ler o rrn até o pipe
                j++;
                aux1[j]=fgetc(arq);
            }
            new_rrn=atoi(aux1);// converte string para int
           
            rrn=realloc(rrn, found*sizeof(int)+sizeof(int)); //realoca o novo rrn
            rrn[found]=new_rrn;
            found++;
        }
        else{
            fprintf(stderr,ANSI_COLOR_RED"Ocorreu algum erro!!"ANSI_COLOR_RESET);
        }
    }
    
    printf("Topo da Ped ="ANSI_COLOR_YELLOW" %d\n"ANSI_COLOR_RESET, rrn[0]);
    free_space=TAM; //quantidade de espaco disponivel apenas para o topo da ped
    printf("Proximos RRNs da PED:\n");

    for(int j=1; j<found; j++){
        printf("RRN[%d] ="ANSI_COLOR_YELLOW" %d\n"ANSI_COLOR_RESET, rrn[j-1], rrn[j]);

        if(rrn[j]!=-1){
            free_space+=TAM; //quantidade de espaco disponivel
        }   
    }

    printf(ANSI_COLOR_GREEN"\nEspaco disponivel: "ANSI_COLOR_YELLOW"[%d]"ANSI_COLOR_RESET, free_space);

    free(rrn);
    fclose(arq);
    return EXIT_SUCCESS;
}

void concatenacao_buffer(char *buffer, FILE *entrada){
    int i, j=0, controlador=0;
    char campo[TAM];
    char aux[TAM];
    int tamanho=0;

    memset(aux,0,strlen(aux));
    
    for(i=0;i<4;i++){ //preciso ler 4 campos para poder terminar um registro
        memset(campo,0,strlen(campo));
        controlador = leia_campo(campo, TAM, entrada);//aqui vai retornar o tamanho do campo lido

        if(controlador > 0){// se for maior que 0 significa que tem algo escrito, se não o campo esta vazio

            strcat(aux, campo); //concateca o campo com o buffer
            strcat(aux, "|"); //concatena a pipe com o campo para mostra o final do me
            
        }
        j++;
    }
    strcpy(buffer, aux); //aux passa o registro pro buffer
}

int importacao_arq(char *filename){
    char nome_arquivo[20];
    FILE *entrada, *saida;
    char buffer[TAM];
    int i, j=0, byteOffset = 4;
    int comp;
 
    entrada = fopen(filename, "r");//vai pegar o arquivo de entrada (.txt) e vai ler suas informações
    saida = fopen("dados.dat", "w");//após o passo a cima ele vai ter que escrever um arquivo novo (.dat)

    if(entrada == NULL){
        printf(ANSI_COLOR_RED"O arquivo ('%s') não foi encontrado."ANSI_COLOR_RESET, nome_arquivo);
        return EXIT_FAILURE;
    }

    i=-1;
    fwrite(&i, sizeof(int), 1, saida);

    while(!feof(entrada)){
        concatenacao_buffer(buffer, entrada);
        fseek(saida, byteOffset, SEEK_SET);
        if(strlen(buffer)<64 && strlen(buffer)!=0){
            comp=strlen(buffer);
            
            for(int i=comp; i<TAM; i++){
                strcat(buffer, ".");
            }
            buffer[64] = '\0';
        }
        fputs(buffer, saida);
        byteOffset+=TAM;
    }
    
    return EXIT_SUCCESS;
}