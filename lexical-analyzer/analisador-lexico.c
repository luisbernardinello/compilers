#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define TAMANHO_PALAVRA 50
#define MAX_SIMBOLOS 1000

// tipos de token
#define TOK_PCHAVE 1
#define TOK_ID 4
#define TOK_NUM 5
#define TOK_PONT 6
#define TOK_OP 7

// valores de palavra chave (palavra reservada)
#define PC_PROGRAM 0
#define PC_VAR 1
#define PC_PROCEDURE 2
#define PC_FUNCTION 3
#define PC_BEGIN 4
#define PC_END 5
#define PC_IF 6
#define PC_THEN 7
#define PC_ELSE 8
#define PC_WHILE 9
#define PC_DO 10
#define PC_REPEAT 11
#define PC_UNTIL 12
#define PC_FOR 13
#define PC_TO 14
#define PC_DOWNTO 15
#define PC_CASE 16
#define PC_OF 17
#define PC_WITH 18
#define PC_ARRAY 19
#define PC_SET 20
#define PC_FILE 21
#define PC_LABEL 22
#define PC_CONST 23
#define PC_TYPE 24
#define PC_PROCEDURE_LABEL 25
#define PC_FORWARD 26
#define PC_EXTERNAL 27
#define PC_LOCAL 28
#define PC_INTEGER 29
#define PC_BOOLEAN 30
#define PC_DIV 31
#define PC_READ 32
#define PC_WRITE 33

// valores de pontuação
#define PARESQ 1
#define PARDIR 2
#define CHVESQ 3
#define CHVDIR 4
#define VIRG 5
#define PNTVIRG 6
#define DPONT 7
#define PONT 8

// valores de operadores
#define SOMA 1
#define SUB 2
#define MULT 3
#define DIVISAO 4
#define MENOR 5
#define IGUAL 6
#define DIF 7
#define MENORIG 8
#define MAIOR 9
#define MAIORIG 10
#define E 11
#define OU 12
#define ATRIB 13

// definição dos tipos
#define NUM 14
#define ID 15
#define STRING 16

// struct de token
typedef struct {
    int tipo;
    int valor;
} Token;

// estrutura para a tabela de símbolos
typedef struct {
    char nome[TAMANHO_PALAVRA];
    int valor;
} Simbolo;

Simbolo tabela_simbolos[MAX_SIMBOLOS];
int contador_simbolos = 0;

int contador_identificador = 0; //identificadores terão valores negativos
char proximo_caractere = ' ';
int linha = 1;
FILE *input;
FILE *output;

void inicializa_analise(char *nome_arq) {
    input = fopen(nome_arq, "r");

    if (input == NULL) {
        fprintf(stderr, "nao foi possivel abrir o arquivo de entrada:%s\n", nome_arq);
        exit(1);
    }
}

// função que avança o caractere
void proximo(FILE *arquivo) {
    char c;
    c = fgetc(arquivo);

    if (c == '\n') {
        linha++;

        printf("analise da linha %d feita sem erros.\n", linha - 1);
        fprintf(output, "LINHA %d:\n  ", linha);
    }
    if (feof(arquivo)) {
        fclose(input);
        exit(0);
    }
    proximo_caractere = toupper(c);
}

// função para tratamento de erro
void erro(const char *mensagem, FILE *arquivo) {
    printf("erro na linha %d: %s\n", linha, mensagem);
    fclose(arquivo);
    exit(1);
}

// função para buscar identificador na tabela de símbolos
int busca_identificador(char *nome) {
    for (int i = 0; i < contador_simbolos; i++) {
        if (strcmp(tabela_simbolos[i].nome, nome) == 0) {
            return tabela_simbolos[i].valor;
        }
    }
    return 0; // Não encontrado
}

// função para adicionar identificador na tabela de símbolos
int adiciona_identificador(char *nome) {
    if (contador_simbolos >= MAX_SIMBOLOS) {
        erro("Tabela de símbolos cheia", input);
    }
    strcpy(tabela_simbolos[contador_simbolos].nome, nome);
    tabela_simbolos[contador_simbolos].valor = contador_identificador--;
    return tabela_simbolos[contador_simbolos++].valor;
}

// função que classifica o lexema e retorna o token
Token *codigo(char *lexema, FILE *output) {
    Token *token = malloc(sizeof(Token));
    if (token == NULL) {
        erro("erro ao alocar memoria", output);
    }

    // regras de lexemas para a linguagem Pascal
    if (strcmp(lexema, "PROGRAM") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_PROGRAM;
    } else if (strcmp(lexema, "VAR") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_VAR;
    } else if (strcmp(lexema, "PROCEDURE") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_PROCEDURE;
    } else if (strcmp(lexema, "FUNCTION") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_FUNCTION;
    } else if (strcmp(lexema, "BEGIN") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_BEGIN;
    } else if (strcmp(lexema, "END") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_END;
    } else if (strcmp(lexema, "IF") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_IF;
    } else if (strcmp(lexema, "THEN") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_THEN;
    } else if (strcmp(lexema, "ELSE") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_ELSE;
    } else if (strcmp(lexema, "WHILE") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_WHILE;
    } else if (strcmp(lexema, "DO") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_DO;
    } else if (strcmp(lexema, "REPEAT") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_REPEAT;
    } else if (strcmp(lexema, "UNTIL") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_UNTIL;
    } else if (strcmp(lexema, "FOR") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_FOR;
    } else if (strcmp(lexema, "TO") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_TO;
    } else if (strcmp(lexema, "DOWNTO") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_DOWNTO;
    } else if (strcmp(lexema, "CASE") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_CASE;
    } else if (strcmp(lexema, "OF") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_OF;
    } else if (strcmp(lexema, "WITH") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_WITH;
    } else if (strcmp(lexema, "ARRAY") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_ARRAY;
    } else if (strcmp(lexema, "SET") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_SET;
    } else if (strcmp(lexema, "FILE") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_FILE;
    } else if (strcmp(lexema, "LABEL") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_LABEL;
    } else if (strcmp(lexema, "CONST") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_CONST;
    } else if (strcmp(lexema, "TYPE") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_TYPE;
    } else if (strcmp(lexema, "PROCEDURE") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_PROCEDURE_LABEL;
    } else if (strcmp(lexema, "FORWARD") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_FORWARD;
    } else if (strcmp(lexema, "EXTERNAL") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_EXTERNAL;
    } else if (strcmp(lexema, "LOCAL") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_LOCAL;
    } else if (strcmp(lexema, "INTEGER") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_INTEGER;
    } else if (strcmp(lexema, "BOOLEAN") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_BOOLEAN;
    } else if (strcmp(lexema, "DIV") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_DIV;
    } else if (strcmp(lexema, "READ") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_READ;
    } else if (strcmp(lexema, "WRITE") == 0) {
        token->tipo = TOK_PCHAVE;
        token->valor = PC_WRITE;
    } else if (strcmp(lexema, "+") == 0) {
        token->tipo = TOK_OP;
        token->valor = SOMA;
    } else if (strcmp(lexema, "-") == 0) {
        token->tipo = TOK_OP;
        token->valor = SUB;
    } else if (strcmp(lexema, "*") == 0) {
        token->tipo = TOK_OP;
        token->valor = MULT;
    } else if (strcmp(lexema, "/") == 0) {
        token->tipo = TOK_OP;
        token->valor = DIVISAO;
    } else if (strcmp(lexema, "<") == 0) {
        token->tipo = TOK_OP;
        token->valor = MENOR;
    } else if (strcmp(lexema, "=") == 0) {
        token->tipo = TOK_OP;
        token->valor = IGUAL;
    } else if (strcmp(lexema, "<>") == 0) {
        token->tipo = TOK_OP;
        token->valor = DIF;
    } else if (strcmp(lexema, "<=") == 0) {
        token->tipo = TOK_OP;
        token->valor = MENORIG;
    } else if (strcmp(lexema, ">") == 0) {
        token->tipo = TOK_OP;
        token->valor = MAIOR;
    } else if (strcmp(lexema, ">=") == 0) {
        token->tipo = TOK_OP;
        token->valor = MAIORIG;
    } else if (strcmp(lexema, "AND") == 0) {
        token->tipo = TOK_OP;
        token->valor = E;
    } else if (strcmp(lexema, "OR") == 0) {
        token->tipo = TOK_OP;
        token->valor = OU;
    } else if (strcmp(lexema, ":=") == 0) {
        token->tipo = TOK_OP;
        token->valor = ATRIB;
    } else if (strcmp(lexema, "(") == 0) {
        token->tipo = TOK_PONT;
        token->valor = PARESQ;
    } else if (strcmp(lexema, ")") == 0) {
        token->tipo = TOK_PONT;
        token->valor = PARDIR;
    } else if (strcmp(lexema, "{") == 0) {
        token->tipo = TOK_PONT;
        token->valor = CHVESQ;
    } else if (strcmp(lexema, "}") == 0) {
        token->tipo = TOK_PONT;
        token->valor = CHVDIR;
    } else if (strcmp(lexema, ",") == 0) {
        token->tipo = TOK_PONT;
        token->valor = VIRG;
    } else if (strcmp(lexema, ";") == 0) {
        token->tipo = TOK_PONT;
        token->valor = PNTVIRG;
    } else if (strcmp(lexema, ":") == 0) {
        token->tipo = TOK_PONT;
        token->valor = DPONT;
    } else if (strcmp(lexema, ".") == 0) {
        token->tipo = TOK_PONT;
        token->valor = PONT;
    } else if (isdigit(lexema[0])) {
        // isdigit para verificar se se trata de um numero
        token->tipo = TOK_NUM;
        token->valor = atoi(lexema);
    } else {
        // verifica se o identificador já existe na tabela de símbolos
        int valor = busca_identificador(lexema);
        if (valor == 0) {
            // se não existir, adiciona na tabela de símbolos
            valor = adiciona_identificador(lexema);
        }
        // se não for palavra chave se trata de um identificador
        token->tipo = TOK_ID;
        token->valor = valor;
    }

    return token;
}

// função para imprimir
void imprime_token(Token *tok, FILE *arquivo) {
    char *tipo;

    switch (tok->tipo) {
        case TOK_NUM:
            tipo = "numero";
            break;

        case TOK_PONT:
            tipo = "pontuacao";
            break;

        case TOK_OP:
            tipo = "operador";
            break;

        case TOK_ID:
            tipo = "identificador";
            break;

        case TOK_PCHAVE:
            tipo = "palavra chave";
            break;

        default:
            tipo = "desconhecido";
    }

    fprintf(arquivo, "tipo: %s - - - - valor: %d\n", tipo, tok->valor);
}

int main() {
    output = fopen("Output.txt", "w");
    fprintf(output, "LINHA 1:\n  ");

    inicializa_analise("Trab1_Compiladores.txt");
    // inicializa_analise("Trab1_Compiladores_ExemploErro.txt");

    if (output == NULL) {
        printf("erro gerado ao abrir o arquivo\n");
        return 1;
    }

    while (!feof(input)) {
        char lexema[TAMANHO_PALAVRA] = {0};
        Token *token = NULL;

        // ignorar os tipos de espaço em branco
        while ((proximo_caractere == ' ' || proximo_caractere == '\t' || proximo_caractere == '\n')) {
            proximo(input);
        }

        if (feof(input)) {
            fclose(output);
            exit(0);
        }

        // isalpha verifica se o próximo caractere é uma letra
        if (isalpha(proximo_caractere)) {
            int i = 0;
            // inicia a formação de um lexema de palavra chave ou identificador
            while (isalpha(proximo_caractere) || isdigit(proximo_caractere)) {
                lexema[i++] = proximo_caractere;
                proximo(input);
            }
            // classifica o lexema como palavra chave ou identificador
            token = codigo(lexema, output);
        } else if (isdigit(proximo_caractere)) {
            int i = 0;
            // inicia a formação de um lexema numero
            while (isdigit(proximo_caractere)) {
                lexema[i++] = proximo_caractere;
                proximo(input);
                if (isalpha(proximo_caractere)) {
                    erro("Identificador mal formado", input);
                }
            }
            // classifica o lexema como numero
            token = codigo(lexema, output);

            //inicio das classificações de simbolos, pontuação e simbolos compostos
        } else if (proximo_caractere == ',' || proximo_caractere == ';' || proximo_caractere == '*') {
            lexema[0] = proximo_caractere;
            lexema[1] = '\0';
            token = codigo(lexema, output);
            // avança o caractere
            proximo(input);
        } else if (proximo_caractere == ':') {
            // simbolo composto de operador de atribuição :=
            proximo(input);
            if (proximo_caractere == '=') {
                lexema[0] = ':';
                lexema[1] = '=';
                lexema[2] = '\0';
                token = codigo(lexema, output);
                // avança o caractere
                proximo(input);
            } else {
                lexema[0] = ':';
                lexema[1] = '\0';
                token = codigo(lexema, output);
            }
        } else if (proximo_caractere == '(') {
            lexema[0] = '(';
            lexema[1] = '\0';
            token = codigo(lexema, output);
            // avança o caractere
            proximo(input);
        } else if (proximo_caractere == ')') {
            lexema[0] = ')';
            lexema[1] = '\0';
            token = codigo(lexema, output);
            // avança o caractere
            proximo(input);
        } else if (proximo_caractere == '=') {
            lexema[0] = '=';
            lexema[1] = '\0';
            token = codigo(lexema, output);
            // avança o caractere
            proximo(input);
        } else if (proximo_caractere == '+') {
            lexema[0] = '+';
            lexema[1] = '\0';
            token = codigo(lexema, output);
            // avança o caractere
            proximo(input);
        } else if (proximo_caractere == '-') {
            lexema[0] = '-';
            lexema[1] = '\0';
            token = codigo(lexema, output);
            // avança o caractere
            proximo(input);
        } else if (proximo_caractere == '<') {
            lexema[0] = '<';
            lexema[1] = '\0';
            token = codigo(lexema, output);
            // avança o caractere
            proximo(input);
        } else if (proximo_caractere == '>') {
            lexema[0] = '>';
            lexema[1] = '\0';
            token = codigo(lexema, output);
            // avança o caractere
            proximo(input);
        } else if (proximo_caractere == '.') {
            lexema[0] = '.';
            lexema[1] = '\0';
            token = codigo(lexema, output);
            // avança o caractere
            proximo(input);
        } else {
            // else se trata de um erro, caractere invalido
            printf("erro durante analise da linha %d: caractere invalido (%c)\n", linha, proximo_caractere);
            fclose(input);
            fclose(output);
            return 1;
        }

        // imprime token
        if (token != NULL) {
            imprime_token(token, output);
            free(token);
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}
