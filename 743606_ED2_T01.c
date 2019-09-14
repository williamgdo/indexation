    /* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 01
 *
 * RA: 743606
 * Aluno: William Giacometti Dutra de Oliveira
 * ========================================================================== */
 
/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
 
 
/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY    11
#define TAM_NOME         51
#define TAM_MARCA         51
#define TAM_DATA         11
#define TAM_ANO         3
#define TAM_PRECO         8
#define TAM_DESCONTO     4
#define TAM_CATEGORIA     51
 
 
#define TAM_REGISTRO     192
#define MAX_REGISTROS     1000
#define MAX_CATEGORIAS     30
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)
 
 
/* Saídas para o usuario */
#define OPCAO_INVALIDA                 "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE         "Memoria insuficiente!"
#define REGISTRO_N_ENCONTRADO         "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO                 "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA            "ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO                 "Arquivo vazio!\n"
#define INICIO_BUSCA                  "**********************BUSCAR**********************\n"
#define INICIO_LISTAGEM              "**********************LISTAR**********************\n"
#define INICIO_ALTERACAO             "**********************ALTERAR*********************\n"
#define INICIO_EXCLUSAO              "**********************EXCLUIR*********************\n"
#define INICIO_ARQUIVO              "**********************ARQUIVO*********************\n"
#define INICIO_ARQUIVO_SECUNDARIO    "*****************ARQUIVO SECUNDARIO****************\n"
#define SUCESSO                       "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA                          "FALHA AO REALIZAR OPERACAO!\n"
 
 
 
/* Registro do Produto */
typedef struct {
    char pk[TAM_PRIMARY_KEY];
    char nome[TAM_NOME];
    char marca[TAM_MARCA];
    char data[TAM_DATA];    /* DD/MM/AAAA */
    char ano[TAM_ANO];
    char preco[TAM_PRECO];
    char desconto[TAM_DESCONTO];
    char categoria[TAM_CATEGORIA];
} Produto;
 
 
/*----- Registros dos Índices -----*/
 
/* Struct para índice Primário */
typedef struct primary_index{
  char pk[TAM_PRIMARY_KEY];
  int rrn;
} Ip;
 
/* Struct para índice secundário */
typedef struct secundary_index{
  char pk[TAM_PRIMARY_KEY];
  char string[TAM_NOME];
} Is;
 
/* Struct para índice secundário de preços */
typedef struct secundary_index_of_final_price{
  float price;
  char pk[TAM_PRIMARY_KEY];
} Isf;
 
/* Lista ligada para o Índice abaixo*/
typedef struct linked_list{
  char pk[TAM_PRIMARY_KEY];
  struct linked_list *prox;
} ll;
 
/* Struct para lista invertida */
typedef struct reverse_index{
  char cat[TAM_CATEGORIA];
  ll* lista;
} Ir;
 
/*----- GLOBAL -----*/
char ARQUIVO[TAM_ARQUIVO];
 
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */
 
/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();
 
/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto);
 
/* Recupera do arquivo o registro com o rrn informado
 *  e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn);
 
/* (Re)faz o índice respectivo */
void criar_iprimary(Ip *indice_primario, int* nregistros);
 
/* Realiza os scanfs na struct Produto */
void ler_entrada(char* registro, Produto *novo);
 
/* Rotina para impressao de indice secundario */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat);
 
 
// MEU /* ========================================================================== */
 
void gerarChave(Produto* p);
void criar_iproduct(Is *isecond, int* nregistros);  //(Re)faz o índice respectivo
void criar_ibrand(Is *isecond, int* nregistros);
void criar_iprice(Isf *isecond, int* nregistros);
void criar_icategory(Ir *ireverse, int* nregistros, int* numCategorias);
void inicializar_listas(Ir *ireverse);
 
void inserir_iprimary(Ip *indice_primario, int* nregistros, Produto p);  // insere um registro e reordena o indice respectivo
void inserir_iproduct(Is *isecond, int* nregistros, Produto p);
void inserir_ibrand(Is *isecond, int* nregistros, Produto p);
void inserir_iprice(Isf *isecond, int* nregistros, Produto p);
void inserir_icategory(Ir *ireverse, int* nregistros, int* numCategorias, Produto p);
 
int busca_chave(char* pk, Ip* iprimary, int nregistros);
int alterar(char *chave, Ip *iprimary, Isf *iprice, int nreg);
int remover(char *chave, Ip *iprimary, int nreg);
void liberarEspaco(Ir *icategory, int* nreg, int* ncat);
 
// FUNCOES DE AUXILIO PARA QSORT E BSEARCH
int comparar_IP(const void *ponteiro1, const void *ponteiro2)
{
    const Ip *a = ponteiro1, *b = ponteiro2;
    return (strcmp(a->pk, b->pk));
}
 
int comparar_CAT(const void *ponteiro1, const void *ponteiro2)
{
    const Ir *a = ponteiro1, *b = ponteiro2;
    return (strcmp(a->cat, b->cat));
}
 
int comparar_IS(const void *ponteiro1, const void *ponteiro2)
{
    const Is *a = ponteiro1, *b = ponteiro2;
    int i = strcmp(a->string, b->string);
    if(i != 0)
          return i;
    else
        return (strcmp(a->pk, b->pk));
}
 
int comparar_IS_busca(const void *ponteiro1, const void *ponteiro2)
{
    const Is *a = ponteiro1, *b = ponteiro2;
    return strcmp(a->string, b->string);
}
 
int comparar_ISF(const void *ponteiro1, const void *ponteiro2)
{
    const Isf *a = ponteiro1, *b = ponteiro2;
    if(a->price != b->price) {
        return (a->price > b->price);
    }
    else {
        return (strcmp(a->pk, b->pk));
    }
}
/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main(){
  /* Arquivo */
    int carregarArquivo = 0, nregistros = 0, ncat = 0;
    scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
    if (carregarArquivo)
        nregistros = carregar_arquivo();
 
    /* Índice primário */
    Ip *iprimary = (Ip *) malloc (MAX_REGISTROS * sizeof(Ip));
      if (!iprimary) {
        perror(MEMORIA_INSUFICIENTE);
        exit(1);
    }
    criar_iprimary(iprimary, &nregistros);
 
    /*Alocar e criar índices secundários*/
    Is *iproduct = (Is *) malloc (MAX_REGISTROS * sizeof(Is));
    Is *ibrand = (Is *) malloc (MAX_REGISTROS * sizeof(Is));
    Isf *iprice = (Isf *) malloc (MAX_REGISTROS * sizeof(Isf));
    Ir *icategory = (Ir *) malloc (MAX_REGISTROS * sizeof(Ir));
    if (!iproduct || !ibrand || !iprice || !icategory) { // usado pra checar se a memoria nao estourou
        perror(MEMORIA_INSUFICIENTE);
        exit(1);
    }
    inicializar_listas(icategory); // preenche os enderecos das listas invertidas com NULL
    criar_iproduct(iproduct, &nregistros);
    criar_ibrand(ibrand, &nregistros);
    criar_iprice(iprice, &nregistros);
    criar_icategory(icategory, &nregistros, &ncat);
 
    Produto p;
    char tmp[TAM_REGISTRO + 1];
    int rrn;
    /* Execução do programa */
    int opcao = 0, argumento;
    while(1)
    {
        scanf("%d%*c", &opcao);
        switch(opcao)
        {
            case 1:
                /*cadastro*/
                ler_entrada(tmp, &p);
                if(busca_chave(p.pk, iprimary, nregistros) == -1) {
                    if(nregistros < MAX_REGISTROS) {
                        strcat(ARQUIVO, tmp);
                        inserir_iprimary(iprimary, &nregistros, p);
                        // atualizar indices secundarios
                        inserir_iproduct(iproduct, &nregistros, p);
                        inserir_ibrand(ibrand, &nregistros, p);
                        inserir_iprice(iprice, &nregistros, p);
                        inserir_icategory(icategory, &nregistros, &ncat, p);
                        nregistros++;
                    }
                }
                else
                    printf(ERRO_PK_REPETIDA, p.pk);
            break;
            case 2:
                /*alterar desconto*/
                printf(INICIO_ALTERACAO);
                scanf("%10[^\n]", p.pk); getchar();
                if(alterar(p.pk, iprimary, iprice, nregistros)) {
                    if(nregistros > 1) // reorganizar para nao listar errado da proxima vez
                        qsort(iprice, nregistros, sizeof(Isf), comparar_ISF);
                    printf(SUCESSO);
                }
                else
                    printf(FALHA);
            break;
            case 3:
                /*excluir produto*/
                printf(INICIO_EXCLUSAO);
                scanf("%10[^\n]", p.pk); getchar();
                if(remover(p.pk, iprimary, nregistros))
                    printf(SUCESSO);
                else
                    printf(FALHA);
            break;
            case 4:
                /*busca*/
                printf(INICIO_BUSCA);
                scanf("%d", &argumento); getchar();
                switch(argumento) {
                    case 1: // busca por codigo
                        scanf("%10[^\n]", p.pk); getchar();
                        rrn = busca_chave(p.pk, iprimary, nregistros);
                        if(rrn != -1)
                            exibir_registro(rrn, 0);
                        else
                            printf(REGISTRO_N_ENCONTRADO);
                    break;
                    case 2: { // busca por nome
                        //int printados[MAX_REGISTROS], cont = 0;
                        Is itemBuscado;
                        scanf("%50[^\n]", itemBuscado.string); getchar();
                        int flag = 1;
                        /*for(int i = 0; i < nregistros; i++) {
                            if(strcmp(itemBuscado.string, iproduct[i].string) == 0) {
                                exibir_registro(busca_chave(iproduct[i].pk, iprimary, nregistros), 0);
                                flag = 0;
                            }
                        }
                        if(flag)
                            printf(REGISTRO_N_ENCONTRADO);
                        break;*/
                        Is* achou = (Is *) bsearch(&itemBuscado, iproduct, nregistros, sizeof(Is), comparar_IS_busca);
                        if(achou != NULL) {
                            Is *start, *end;
                            end = achou;

                            while(1) { // encontra o primeiro registro com o nome escrito
                                achou--;
                                if(strcmp(itemBuscado.string, achou->string) != 0) {
                                    achou++;
                                    start = achou;
                                    break;
                                }
                            }
                            while(1) { // encontra o ultimo registro com o nome escrito
                                end++;
                                if(strcmp(itemBuscado.string, end->string) != 0) {
                                    end--;
                                    break;
                                }
                            }
                            
                            while(start != end) {
                                rrn = busca_chave(start->pk, iprimary, nregistros);
                                if(rrn != -1) {
                                    /*for(int i = 0; i < cont; i++) {
                                        if(printados[i] == rrn) {

                                        }
                                    }
                                    printados[cont] = rrn;
                                    cont++;*/
                                    exibir_registro(busca_chave(start->pk, iprimary, nregistros), 0);
                                    if(busca_chave(end->pk, iprimary, nregistros) != -1)
                                        printf("\n");

                                    flag = 0;
                                }
                                start++;
                            }
                            rrn = busca_chave(end->pk, iprimary, nregistros);
                            if(rrn != -1) {
                                exibir_registro(busca_chave(end->pk, iprimary, nregistros), 0);
                                flag = 0;
                            }
                            if(flag)
                                printf(REGISTRO_N_ENCONTRADO);
                        } 
                        else
                            printf(REGISTRO_N_ENCONTRADO);
                        break;
                    }
                    case 3: // buscar por categoria
                        scanf("%50[^\n]", p.marca); getchar();
                        //char *achou = bsearch(p.marca, ibrand, nregistros, sizeof(Is), comparar_IS_busca);
                        scanf("%50[^\n]", p.categoria); getchar();
                        Ir *tmp = bsearch(p.categoria, icategory, ncat, sizeof(Ir), comparar_CAT); // procura a categoria
                        if(tmp) {
                            ll *aux = tmp->lista; // auxiliar pra percorrer a lista inversa
                            Produto b;
                            int flag = 1;
                            while(aux) {
                                rrn = busca_chave(aux->pk, iprimary, nregistros);                        
                                if(rrn != -1) {
                                    b = recuperar_registro(rrn);
                                    if(strcmp(b.marca, p.marca) == 0) {
                                        if(!flag)
                                            printf("\n"); 
                                        else
                                            flag = 0;
                                        exibir_registro(rrn, 0);
                                    }
                                    // exibir_registro(busca_chave(aux->pk, iprimary, nregistros), 0);
                                }
                                aux = aux->prox;
                            }
                            if(flag)
                                printf(REGISTRO_N_ENCONTRADO);
                        }
                        else
                            printf(REGISTRO_N_ENCONTRADO);
                    break;
                }
            break;
            case 5:
                /*listagens*/
                printf(INICIO_LISTAGEM);
                scanf("%d%*c", &argumento);
                switch(argumento) {
                    case 1: // listar por codigo
                        if(nregistros == 0)
                            printf(REGISTRO_N_ENCONTRADO);
                        else {
                            int flag = 1;
                            for(int i = 0; i < nregistros ; i++) {
                                if(iprimary[i].rrn != -1) {
                                    flag = 0;
                                    exibir_registro(iprimary[i].rrn, 0);
                                    if(i != nregistros - 1)
                                        printf("\n");
                                }
                            }
                            if(flag)
                                printf(REGISTRO_N_ENCONTRADO);
                        }
                    break;
                    case 2: // listar por categoria
                        scanf("%50[^\n]", p.categoria);
                        Ir *tmp = bsearch(p.categoria, icategory, ncat, sizeof(Ir), comparar_CAT);
                        int flag = 1;
                        if(tmp) {
                            ll *aux = tmp->lista;
                            while(aux) {
                                rrn = busca_chave(aux->pk, iprimary, nregistros);
                                if(rrn != -1) {
                                    flag = 0;
                                    exibir_registro(rrn, 0);
                                    if(aux->prox != NULL)
                                        printf("\n");
                                }
                                aux = aux->prox;
                            }
                            if(flag)
                                printf(REGISTRO_N_ENCONTRADO);
                        }
                        else
                            printf(REGISTRO_N_ENCONTRADO);
                    break;
                    case 3: // listar por marca
                        if(nregistros == 0)
                            printf(REGISTRO_N_ENCONTRADO);
                        else {
                            int flag = 1;
                            for(int i = 0; i < nregistros ; i++) {
                                rrn = busca_chave(ibrand[i].pk, iprimary, nregistros);
                                if(rrn != -1) {
                                    flag = 0;
                                    exibir_registro(rrn, 0);
                                    if(i != nregistros - 1)
                                        printf("\n");
                                }
                            }
                            if(flag)
                                printf(REGISTRO_N_ENCONTRADO);
                        }
                    break;
                    case 4: // listar por preco
                        if(nregistros == 0)
                            printf(REGISTRO_N_ENCONTRADO);
                        else {
                            int flag = 1;
                            for(int i = 0; i < nregistros ; i++) {
                                rrn = busca_chave(iprice[i].pk, iprimary, nregistros);
                                if(rrn != -1) {
                                    flag = 0;
                                    exibir_registro(rrn, 1);
                                    if(i != nregistros - 1)
                                        printf("\n");
                                }
                            }
                            if(flag)
                                printf(REGISTRO_N_ENCONTRADO);
                        }
                    break;
                }
            break;
            case 6:
            {
                /*libera espaço*/
                free(iprimary); // libera a memoria para recriar apos reconfigurar o arquivo
                free(iproduct);
                free(ibrand);
                free(iprice);
                free(icategory);
                liberarEspaco(icategory, &nregistros, &ncat);
                Ip *iprimary = (Ip *) malloc (MAX_REGISTROS * sizeof(Ip));
                Is *iproduct = (Is *) malloc (MAX_REGISTROS * sizeof(Is));
                Is *ibrand = (Is *) malloc (MAX_REGISTROS * sizeof(Is));
                Isf *iprice = (Isf *) malloc (MAX_REGISTROS * sizeof(Isf));
                Ir *icategory = (Ir *) malloc (MAX_REGISTROS * sizeof(Ir));
                inicializar_listas(icategory); // preenche os enderecos das listas invertidas com NULL
                criar_iprimary(iprimary, &nregistros); // recria os indices excluidos
                criar_ibrand(ibrand, &nregistros);
                criar_icategory(icategory, &nregistros, &ncat);
                criar_iprice(iprice, &nregistros);
                criar_iproduct(iproduct, &nregistros);
            }
            break;
            case 7:
                /*imprime o arquivo de dados*/
                printf(INICIO_ARQUIVO);
                if(strlen(ARQUIVO) > 0)
                    printf("%s\n", ARQUIVO);
                else
                    printf(ARQUIVO_VAZIO);
            break;
            case 8:
                /*imprime os índices secundários*/
                imprimirSecundario(iproduct, ibrand, icategory, iprice, nregistros, ncat);
            break;
            case 9:
                /*Liberar memória e finalizar o programa */
                free(iprimary); // libera a memoria por bom uso
                free(iproduct);
                free(ibrand);
                free(iprice);
                free(icategory);
                liberarEspaco(icategory, &nregistros, &ncat);
                return 0;
            break;
            default:
                printf(OPCAO_INVALIDA);
            break;
        }
    }
    return 0;
}
 
/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto)
{
    if(rrn<0)
        return 0;
    float preco;
    int desconto;
    Produto j = recuperar_registro(rrn);
    char *cat, categorias[TAM_CATEGORIA];
    printf("%s\n", j.pk);
    printf("%s\n", j.nome);
    printf("%s\n", j.marca);
    printf("%s\n", j.data);
    if(!com_desconto)
    {
        printf("%s\n", j.preco);
        printf("%s\n", j.desconto);
    }
    else
    {
        sscanf(j.desconto,"%d",&desconto);
        sscanf(j.preco,"%f",&preco);
        preco = preco *  (100-desconto);
        preco = ((int) preco)/ (float) 100 ;
        printf("%07.2f\n",  preco);
 
    }
    strcpy(categorias, j.categoria);
 
    cat = strtok (categorias, "|");
 
    while(cat != NULL){
        printf("%s", cat);
        cat = strtok (NULL, "|");
        if(cat != NULL){
            printf(", ");
        }
    }
 
    printf("\n");
 
    return 1;
}
 
int carregar_arquivo()
{
    scanf("%[^\n]\n", ARQUIVO);
    return strlen(ARQUIVO) / TAM_REGISTRO;
}
 
/* Recupera do arquivo o registro com o rrn
 * informado e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn)
{
    char temp[193], *p;
    strncpy(temp, ARQUIVO + ((rrn)*192), 192);
    temp[192] = '\0';
    Produto j;
    p = strtok(temp,"@");
    strcpy(j.nome,p);
    p = strtok(NULL,"@");
    strcpy(j.marca,p);
    p = strtok(NULL,"@");
    strcpy(j.data,p);
    p = strtok(NULL,"@");
    strcpy(j.ano,p);
    p = strtok(NULL,"@");
    strcpy(j.preco,p);
    p = strtok(NULL,"@");
    strcpy(j.desconto,p);
    p = strtok(NULL,"@");
    strcpy(j.categoria,p);
    gerarChave(&j);
    return j;
}
 
/* Imprimir indices secundarios */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat){
    int opPrint = 0;
    ll *aux;
    printf(INICIO_ARQUIVO_SECUNDARIO);
    scanf("%d", &opPrint);
    if(!nregistros) {
        printf(ARQUIVO_VAZIO);
    }
    switch (opPrint) {
        case 1:
            for(int i = 0; i < nregistros; i++){
                printf("%s %s\n",iproduct[i].pk, iproduct[i].string);
            }
        break;
        case 2:
            for(int i = 0; i < nregistros; i++){
                printf("%s %s\n",ibrand[i].pk, ibrand[i].string);
            }
        break;
        case 3:
            for(int i = 0; i < ncat; i++){
                printf("%s", icategory[i].cat);
                aux =  icategory[i].lista;
                while(aux != NULL){
                    printf(" %s", aux->pk);
                    aux = aux->prox;
                }
                printf("\n");
            }
        break;
 
        case 4:
        for(int i = 0; i < nregistros; i++){
            printf("%s %.2f\n",iprice[i].pk, iprice[i].price);
        }
        break;
    }
}
 
void criar_iprimary(Ip *indice_primario, int* nregistros)
{
    if(*nregistros > 0) { 
        Produto p;
        for(int i = 0; i < *nregistros; i++) {
            p = recuperar_registro(i);
            strcpy(indice_primario[i].pk, p.pk);
            indice_primario[i].rrn = i;
        }
 
        if(*nregistros > 1)
            qsort(indice_primario, *nregistros, sizeof(Ip), comparar_IP);
    }
}
 
void gerarChave(Produto* p)
{
    char newChave[TAM_PRIMARY_KEY];
    strncpy(newChave, p->nome, 2); // copia os primeiros dois digitos de Nome para newChave
    strncpy(newChave+2, p->marca, 2); // copia os primeiros dois digitos de marca a partir da posicao 2 de newChave
    strncpy(newChave+4, p->data, 2); // copia os primeiros dois digitos de data a a partir da posicao 4 de newChave
    strncpy(newChave+6, p->data+3, 2);
    strncpy(newChave+8, p->ano, 2);
    strncpy(p->pk, newChave, TAM_PRIMARY_KEY); // copia todos digitos
    p->pk[TAM_PRIMARY_KEY-1] = '\0';
}
 
void ler_entrada(char* registro, Produto *novo)
{
    scanf("%50[^\n]", novo->nome); getchar();
    scanf("%50[^\n]", novo->marca); getchar();
    scanf("%[^\n]", novo->data); getchar();
    scanf("%[^\n]", novo->ano); getchar();
    scanf("%[^\n]", novo->preco); getchar();
    scanf("%[^\n]", novo->desconto); getchar();
    scanf("%50[^\n]", novo->categoria); getchar();
    gerarChave(novo);
 
    char string_arquivo[TAM_REGISTRO + 1], *arroba;
    int i = 0, j;
 
    i = sprintf(string_arquivo, "%s@%s@%s@%s@%s@%s@%s@", novo->nome, novo->marca, novo->data, novo->ano, novo->preco, novo->desconto, novo->categoria);
 
    if(i < TAM_REGISTRO) // preenche o string de # ate acabar
        for(j = i; j < TAM_REGISTRO; j++)
            string_arquivo[j] = '#';
    string_arquivo[j] = '\0';
 
    strcpy(registro, string_arquivo);
}
 
void criar_iproduct(Is *isecond, int* nregistros)
{
    if(*nregistros > 0) {
        Produto p;
        for(int i = 0; i < *nregistros; i++) {
            p = recuperar_registro(i);
            strcpy(isecond[i].pk, p.pk);
            strcpy(isecond[i].string, p.nome);
        }
 
        if(*nregistros > 1)
            qsort(isecond, *nregistros, sizeof(Is), comparar_IS);
    }
}
 
void criar_ibrand(Is *isecond, int* nregistros)
{
    if(*nregistros > 0) {
        Produto p;
        for(int i = 0; i < *nregistros; i++) {
            p = recuperar_registro(i);
            strcpy(isecond[i].pk, p.pk);
            strcpy(isecond[i].string, p.marca);
        }
 
        if(*nregistros > 1)
            qsort(isecond, *nregistros, sizeof(Is), comparar_IS);
    }
}
 
void criar_iprice(Isf *isecond, int* nregistros)
{
    if(*nregistros > 0) {
        Produto p;
        int desconto;
        float preco;
        for(int i = 0; i < *nregistros; i++) {
            p = recuperar_registro(i);
            strcpy(isecond[i].pk, p.pk);
            sscanf(p.desconto, "%d", &desconto);
            sscanf(p.preco, "%f", &preco);
            preco = (preco * (100-desconto)) / 100.0;
            preco *= 100;
            isecond[i].price = ((int) preco)/ (float) 100.0;
        }
 
        if(*nregistros > 1)
            qsort(isecond, *nregistros, sizeof(Isf), comparar_ISF);
    }
}
 
void inicializar_listas(Ir *ireverse)
{
    for(int i = 0; i < MAX_REGISTROS; i++)
    {
        ireverse[i].lista = NULL;
        strcpy(ireverse[i].cat, "\0");
    }
}
 
void criar_icategory(Ir *ireverse, int* nregistros, int* numCategorias)
{
    *numCategorias = 0;
    if(*nregistros > 0) {
        for(int i = 0; i < *nregistros; i++) {
            Produto p = recuperar_registro(i);
            char *pedaco, *busca;
            pedaco = strtok(p.categoria, "|"); // passa o endereco da string e obtem a primeira categoria
            while (pedaco != NULL) {
                busca = bsearch(pedaco, ireverse, *numCategorias, sizeof(Ir), comparar_CAT); // procura a categoria no vetor
 
                if(busca == NULL) { // se busca for 0, a categoria ainda nao existe
                    strcpy(ireverse[*numCategorias].cat, pedaco); // cria a categoria
                    ll *aux = (ll *) malloc (sizeof(ll)); // instancia e coloca os dados na lista
                    strcpy(aux->pk, p.pk);
                    aux->prox = NULL;
                    ireverse[*numCategorias].lista = aux; // linka a lista na categoria no vetor
                    (*numCategorias)++;
                }
                else {
                    int j = 0;
                    while(strcmp(pedaco, ireverse[j].cat) != 0) // encontra a categoria que o pedaco foi criado
                        j++;
 
                    ll *tmp = ireverse[j].lista;
 
                    if(strcmp(p.pk, tmp->pk) < 0) { // caso seja menor que o primeiro, substitui ele
                        ll *aux = (ll *) malloc (sizeof(ll)); 
                        strcpy(aux->pk, p.pk);
                        aux->prox = ireverse[j].lista;
                        ireverse[j].lista = aux;
                    }
                    else {
                        ll *anterior = ireverse[j].lista;
                        int flag = 1;
                        tmp = tmp->prox;
                        
                        if(tmp != NULL) {
                            while(strcmp(p.pk, tmp->pk) >= 0) { // procura o menor item da lista
                                tmp = tmp->prox;
                                anterior = anterior->prox;
                                if(tmp == NULL) {
                                    break;
                                    flag = 0;
                                }    
                            }
                        }
                        else
                            flag = 0;
                        if(flag) {
                            ll *aux = (ll *) malloc (sizeof(ll)); // instancia e coloca os dados na lista
                            strcpy(aux->pk, p.pk);
                            anterior->prox = aux; // insere no meio de dois
                            aux->prox = tmp;
                        }
                        else {
                            ll *aux = (ll *) malloc (sizeof(ll)); // instancia e coloca os dados na lista
                            strcpy(aux->pk, p.pk);
                            anterior->prox = aux; // insere no final
                            aux->prox = NULL; // o ultimo tem que apontar pra nulo
                        }
                    }            
                }
                pedaco = strtok(NULL, "|");
                if(*numCategorias > 1) // reordena pro bsearch poder funcionar
                    qsort(ireverse, *numCategorias, sizeof(Ir), comparar_CAT);
            }
        }
    }
}
 
int busca_chave(char* pk, Ip* iprimary, int nregistros)
{
    Ip tmp, *aux;
    strcpy(tmp.pk, pk);
    aux = bsearch(&tmp, iprimary, nregistros, sizeof(Ip), comparar_IP);
 
    if(aux != NULL)
        return aux->rrn;
    else
        return -1;
}
 
char* busca_categoria(char* nome, Is* isecundary, int nregistros)
{
    Is* tmp;
    tmp = bsearch(nome, isecundary, nregistros, sizeof(Is), comparar_IS);
 
    return tmp->pk;
}
 
void inserir_iproduct(Is *isecond, int* nregistros, Produto p)
{
    strcpy(isecond[*nregistros].pk, p.pk);
    strcpy(isecond[*nregistros].string, p.nome);
 
    if(*nregistros + 1 > 1)
        qsort(isecond, *nregistros + 1, sizeof(Is), comparar_IS);
}
 
void inserir_ibrand(Is *isecond, int* nregistros, Produto p)
{
    strcpy(isecond[*nregistros].pk, p.pk);
    strcpy(isecond[*nregistros].string, p.marca);
 
    if(*nregistros + 1 > 1)
        qsort(isecond, *nregistros + 1, sizeof(Is), comparar_IS);
}
 
void inserir_iprice(Isf *isecond, int* nregistros, Produto p)
{
    int desconto;
    float preco;
 
    strcpy(isecond[*nregistros].pk, p.pk);
    sscanf(p.desconto, "%d", &desconto);
    sscanf(p.preco, "%f", &preco);
    preco = (preco * (100 - desconto)) / 100.0;
    preco *= 100;
    isecond[*nregistros].price = ((int) preco) / (float) 100.0;
 
    if(*nregistros + 1 > 1)
        qsort(isecond, *nregistros + 1, sizeof(Isf), comparar_ISF);
}
 
void inserir_iprimary(Ip *indice_primario, int* nregistros, Produto p)
{
    strcpy(indice_primario[*nregistros].pk, p.pk);
    indice_primario[*nregistros].rrn = *nregistros;
 
    if(*nregistros + 1 > 1)
        qsort(indice_primario, (*nregistros) + 1, sizeof(Ip), comparar_IP);
}
 
void inserir_icategory(Ir *ireverse, int* nregistros, int* numCategorias, Produto p)
{
    char *pedaco, *busca;
    pedaco = strtok(p.categoria, "|"); // passa o endereco da string e obtem a primeira categoria
    while (pedaco != NULL) {
        busca = bsearch(pedaco, ireverse, *numCategorias, sizeof(Ir), comparar_CAT); // procura a categoria no vetor
 
        if(busca == NULL) { // se busca for 0, a categoria ainda nao existe
            strcpy(ireverse[*numCategorias].cat, pedaco); // cria a categoria
            ll *aux = (ll *) malloc (sizeof(ll)); // instancia e coloca os dados na lista
            strcpy(aux->pk, p.pk);
            aux->prox = NULL;
            ireverse[*numCategorias].lista = aux; // linka a lista na categoria no vetor
            (*numCategorias)++;
        }
        else {
            int j = 0;
            while(strcmp(pedaco, ireverse[j].cat) != 0) // encontra a categoria que o pedaco foi criado
                j++;
 
            ll *tmp = ireverse[j].lista;
 
            if(strcmp(p.pk, tmp->pk) < 0) {
                ll *aux = (ll *) malloc (sizeof(ll));
                strcpy(aux->pk, p.pk);
                aux->prox = ireverse[j].lista;
                ireverse[j].lista = aux;
            }
            else {
                ll *anterior = ireverse[j].lista;
                int flag = 1;
                tmp = tmp->prox;
                
                if(tmp != NULL) {
                    while(strcmp(p.pk, tmp->pk) >= 0) {// procura o menor item da lista
                        tmp = tmp->prox;
                        anterior = anterior->prox;
                        if(tmp == NULL) {
                            break;
                            flag = 0;
                        }    
                    }
                }
                else
                    flag = 0;
                    
                
                if(flag) {
                    ll *aux = (ll *) malloc (sizeof(ll)); // instancia e coloca os dados na lista
                    strcpy(aux->pk, p.pk);
                    anterior->prox = aux; // insere no meio de dois
                    aux->prox = tmp;
                }
                else {
                    ll *aux = (ll *) malloc (sizeof(ll)); // instancia e coloca os dados na lista
                    strcpy(aux->pk, p.pk);
                    anterior->prox = aux; // insere no final
                    aux->prox = NULL; // o ultimo tem que apontar pra nulo
                }
            }            
        }
        pedaco = strtok(NULL, "|");
        if(*numCategorias > 1) 
            qsort(ireverse, *numCategorias, sizeof(Ir), comparar_CAT);
    }    
}
 
int alterar(char *chave, Ip *iprimary, Isf *iprice, int nreg)
{
    char desconto[TAM_DESCONTO];
    int rrn = busca_chave(chave, iprimary, nreg), desc;
    if(rrn != -1) {
        while(1) {
            scanf("%3[^\n]", desconto); getchar();
            sscanf(desconto, "%d", &desc);
            if(desc >= 0 && desc <= 100)
                break;
            else
                printf(CAMPO_INVALIDO);
        }
        char tmp[TAM_REGISTRO + 1];
        int cont = 5, i = 0;
        float precoNovo = -1;
        Produto p = recuperar_registro(rrn);
        strncpy(tmp, ARQUIVO + (TAM_REGISTRO*rrn), TAM_REGISTRO);
        while(cont) {
            if(tmp[i] == '@')
                cont--;
            if(precoNovo == -1 && cont == 1) {
                sscanf(tmp + i + 1, "%f", &precoNovo);
            }
            i++;
        }
        strncpy(ARQUIVO + (TAM_REGISTRO*rrn) + i, desconto, 3);
        i = 0;
        while(i < nreg) {
            if(strcmp(iprice[i].pk, chave) == 0) {
                precoNovo = (precoNovo * (100 - desc)) / 100.0;
                precoNovo *= 100; // usado pra arrumar o arredondamento, compartilhado pelos monitores
                iprice[i].price = (int) precoNovo / (float) 100.0;
            }
            i++;
        }
        return 1;
    }
    else {
        printf(REGISTRO_N_ENCONTRADO);
        return 0;
    }
}
 
int remover(char *chave, Ip *iprimary, int nreg)
{
    Ip tmp, *aux;
    strcpy(tmp.pk, chave); 
    aux = bsearch(&tmp, iprimary, nreg, sizeof(Ip), comparar_IP);
    
    if(aux) {
        if(aux->rrn != -1) {
            strncpy(ARQUIVO + (aux->rrn*TAM_REGISTRO), "*|", 2);
            aux->rrn = -1;
            return 1;
        }
    }
    printf(REGISTRO_N_ENCONTRADO);
    return 0;
}
 
void liberarEspaco(Ir *icategory, int* nreg, int* ncat)
{
    for(int i = 0; i < *ncat; i++) { // libera cada um dos nos da lista criados dinamicamente
        ll *aux = icategory[i].lista;
        while(aux) {
            ll *prox = aux->prox;
            free(aux);
            aux = prox;
        }
    }

    char arquivoNovo[TAM_ARQUIVO + 1], tmp[TAM_REGISTRO + 1];
    arquivoNovo[0] = '\0';
    for(int i = 0; i < *nreg; i++) { 
        strncpy(tmp, ARQUIVO + TAM_REGISTRO*i, TAM_REGISTRO); // copia o que nao comeca com *| pra um novo string 
        if(tmp[0] == '*' && tmp[1] == '|')
            continue;
        else
            strcat(arquivoNovo, tmp); 
    }
    strcpy(ARQUIVO, arquivoNovo);
    *nreg = strlen(ARQUIVO) / TAM_REGISTRO;
}