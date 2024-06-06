#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* 

// valores de palavra chave (palavra reservada) PC_

palavra chave PROGRAM 0
palavra chave  VAR 1
palavra chave  PROCEDURE 2
palavra chave  FUNCTION 3
palavra chave  BEGIN 4
palavra chave  END 5
palavra chave  IF 6
palavra chave  THEN 7
palavra chave ELSE 8
palavra chave WHILE 9
palavra chave  DO 10
palavra chave REPEAT 11
palavra chave UNTIL 12
palavra chave FOR 13
palavra chave TO 14
palavra chave DOWNTO 15
palavra chave CASE 16
palavra chave OF 17
palavra chave WITH 18
palavra chave ARRAY 19
palavra chave SET 20
palavra chave FILE 21
palavra chave LABEL 22
palavra chave CONST 23
palavra chave TYPE 24
palavra chave PROCEDURE_LABEL 25
palavra chave FORWARD 26
palavra chave EXTERNAL 27
palavra chave LOCAL 28
palavra chave INTEGER 29
palavra chave BOOLEAN 30
palavra chave DIV 31


// valores de pontuação PONT_

parenteses esquerdo valor 1
parenteses direito valor 2
chaves esquerda 3
chaves direita 4
virgula 5
ponto e virgula 6
dois pontos 7
ponto 8

// valores de operadores OPER_

SOMA 1
subtração 2
multiplicação 3
divisão 4
menor 5
igual 6
diferente 7
menor e igual8
maior 9
maior e igual 10
E 11
OU 12
atribuição 13

Números NUM_

Identificadores IDENT_

*/

int linha = 0;
FILE *input;

// tipos de token
enum TipoToken {
    OPER_,
    PONT_,
    PC_,
    IDENT_,
    NUM_,
    IDENT_PROCED_
};

// struct para o token
typedef struct {
    enum TipoToken tipo;
    char valor[50];
} Token;

typedef struct {
    char identificadores[100][50]; // limite da tabela de simbolos
    int tipo[100]; // tipo correspondente do identificador
    int tamanho;
} TabelaSimbolos;

void inicializa_analise(char *nome_arq);
void extrai_valor_token(FILE *arquivo, Token *token);
void gera_erro(const char *desc_erro, int linha);
void proximo_token(FILE *input, Token *token, TabelaSimbolos *tabela);
bool verifica_numero(Token *token);
bool identificador(Token *token);
bool identificador_procedure(Token *token);
void cria_identificador_procedure(Token *token, TabelaSimbolos *tabela);
bool verificar_identificador(TabelaSimbolos *tabela, char *identificador, enum TipoToken *tipo);

// funções da gramática Pascal simplificada
void programa(FILE *input, Token *token, TabelaSimbolos *tabela);
void bloco(FILE *input, Token *token, TabelaSimbolos *tabela);
void parte_decl_var(FILE *input, Token *token, TabelaSimbolos *tabela);
void decl_var(FILE *input, Token *token, TabelaSimbolos *tabela);
void lista_ident(FILE *input, Token *token, TabelaSimbolos *tabela);
void tipo(FILE *input, Token *token, TabelaSimbolos *tabela);
void parte_decl_subrotinas(FILE *input, Token *token, TabelaSimbolos *tabela);
void decl_procedimento(FILE *input, Token *token, TabelaSimbolos *tabela);
void parametros_formais(FILE *input, Token *token, TabelaSimbolos *tabela);
void secao_parametros_formais(FILE *input, Token *token, TabelaSimbolos *tabela);
void comando_composto(FILE *input, Token *token, TabelaSimbolos *tabela);
void comando(FILE *input, Token *token, TabelaSimbolos *tabela);
void atribuicao(FILE *input, Token *token, TabelaSimbolos *tabela);
void chamada_procedimento(FILE *input, Token *token, TabelaSimbolos *tabela);
void comando_condicional(FILE *input, Token *token, TabelaSimbolos *tabela);
void comando_repetitivo(FILE *input, Token *token, TabelaSimbolos *tabela);
void expressao(FILE *input, Token *token, TabelaSimbolos *tabela);
void relacao(FILE *input, Token *token, TabelaSimbolos *tabela);
void expressao_simples(FILE *input, Token *token, TabelaSimbolos *tabela);
void termo(FILE *input, Token *token, TabelaSimbolos *tabela);
void fator(FILE *input, Token *token, TabelaSimbolos *tabela);

void inicializa_analise(char *nome_arq) {
    input = fopen(nome_arq, "r");

    if (input == NULL) {
        printf("Nao foi possivel abrir o arquivo de entrada: %s\n", nome_arq);
        exit(1);
    }
}

void extrai_valor_token(FILE *arquivo, Token *token) {
    char buffer[1024];
    char *posicao;

    while (fgets(buffer, sizeof(buffer), arquivo) != NULL) {
        if (strstr(buffer, "LINHA") != NULL) {
            linha++;
            continue;
        }

        if (strstr(buffer, "palavra chave") != NULL) {
            token->tipo = PC_;
        } else if (strstr(buffer, "identificador") != NULL) {
            token->tipo = IDENT_;
        }  else if (strstr(buffer, "numero") != NULL) {
            token->tipo = NUM_;
        } else if (strstr(buffer, "operador") != NULL) {
            token->tipo = OPER_;
        } else if (strstr(buffer, "pontuacao") != NULL) {
            token->tipo = PONT_;
        } else {
            continue;
        }

        posicao = strstr(buffer, "valor:");
        if (posicao != NULL) {
            posicao += strlen("valor: ");
            char *valor_str = strtok(posicao, "\n");
            if (valor_str != NULL) {
                strcpy(token->valor, valor_str);
                return;
            }
        }
    }
}

void gera_erro(const char *desc_erro, int linha) {
    fprintf(stderr, "Erro sintatico na linha %d: %s\n", linha, desc_erro);
    exit(1);
}

bool verifica_numero(Token *token) {
    return token->tipo == NUM_;
}

bool identificador(Token *token) {
    return token->tipo == IDENT_;
}

bool identificador_procedure(Token *token) {
    return token->tipo == IDENT_PROCED_;
}

void cria_identificador_procedure(Token *token, TabelaSimbolos *tabela) {
    // Verifica se o identificador já está na tabela de símbolos
    for (int i = 0; i < tabela->tamanho; i++) {
        if (strcmp(tabela->identificadores[i], token->valor) == 0) {
            // Se o identificador já estiver na tabela, altera o tipo do token na tabela
            // para IDENT_PROCED_
            tabela->tipo[i] = IDENT_PROCED_;
            token->tipo = IDENT_PROCED_;
            return;
        }
    }

    // Se não estiver na tabela, insere-o como identificador de procedure
    strcpy(tabela->identificadores[tabela->tamanho], token->valor);
    tabela->tipo[tabela->tamanho] = IDENT_PROCED_;
    tabela->tamanho++;
    token->tipo = IDENT_PROCED_;
}

bool verificar_identificador(TabelaSimbolos *tabela, char *identificador, enum TipoToken *tipo) {
    for (int i = 0; i < tabela->tamanho; i++) {
        if (strcmp(tabela->identificadores[i], identificador) == 0) {
            *tipo = tabela->tipo[i];
            return true; // Identificador encontrado
        }
    }
    return false; // Identificador não encontrado
}

void proximo_token(FILE *input, Token *token, TabelaSimbolos *tabela) {
    extrai_valor_token(input, token);

    printf("Linha %d: Token { tipo: %d, valor: %s }\n", linha, token->tipo, token->valor);

    // Verifica se o token é um identificador
    if (token->tipo == IDENT_) {
        enum TipoToken tipo;
        // Verifica se o identificador já está na tabela de símbolos
        if (verificar_identificador(tabela, token->valor, &tipo)) {
            // Se estiver na tabela, atualiza o tipo do token
            token->tipo = tipo;
        } else {
            // Se não estiver na tabela, insere-o
            strcpy(tabela->identificadores[tabela->tamanho], token->valor);
            tabela->tipo[tabela->tamanho] = IDENT_;
            tabela->tamanho++;
        }
    }
}

void programa(FILE *input, Token *token, TabelaSimbolos *tabela) {
    proximo_token(input, token, tabela);
    if (token->tipo == PC_ && strcmp(token->valor, "0") == 0) { // program
        proximo_token(input, token, tabela);
        if (identificador(token)) {
            proximo_token(input, token, tabela);
            if (token->tipo == PONT_ && strcmp(token->valor, "6") == 0) { // ;
                proximo_token(input, token, tabela);
                bloco(input, token, tabela);
                if (token->tipo == PONT_ && strcmp(token->valor, "8") == 0) { // .
                    printf("Analise sintatica concluida com sucesso.\n");
                    return;
                } else {
                    gera_erro("Ponto final esperado no final do programa.", linha);
                }
            } else {
                gera_erro("Ponto e virgula esperado apos o identificador do programa.", linha);
            }
        } else {
            gera_erro("Identificador esperado apos 'program'.", linha);
        }
    } else {
        gera_erro("'program' esperado no inicio do programa.", linha);
    }
}

void bloco(FILE *input, Token *token, TabelaSimbolos *tabela) {
    if (token->tipo == PC_ && strcmp(token->valor, "1") == 0) { // var
        parte_decl_var(input, token, tabela);
    }
    if (token->tipo == PC_ && strcmp(token->valor, "2") == 0) { // procedure
        parte_decl_subrotinas(input, token, tabela);
    }
    if (token->tipo == PC_ && strcmp(token->valor, "4") == 0) { // begin
		comando_composto(input, token, tabela);

    }
	if (token->tipo == PONT_ && strcmp(token->valor, "8") == 0) { // temos um . retorna para o funcao principal
        return;
    }
	  else {
        gera_erro("'begin' esperado para iniciar um bloco.", linha);
    }
}

void parte_decl_var(FILE *input, Token *token, TabelaSimbolos *tabela) {
    if (token->tipo == PC_ && strcmp(token->valor, "1") == 0) { // var
        proximo_token(input, token, tabela);
        decl_var(input, token, tabela);
        while (token->tipo == PONT_ && strcmp(token->valor, "6") == 0) { // ;
            proximo_token(input, token, tabela);
            if (token->tipo == PC_ && (strcmp(token->valor, "2") == 0 || strcmp(token->valor, "4") == 0)) { // procedure e begin
                return;
            }
            decl_var(input, token, tabela);
        }
    }
}

void decl_var(FILE *input, Token *token, TabelaSimbolos *tabela) {
    lista_ident(input, token, tabela);
    if (token->tipo == PONT_ && strcmp(token->valor, "7") == 0) { // :
        proximo_token(input, token, tabela);
        tipo(input, token, tabela);
    } else {
        gera_erro("Dois pontos esperado apos a lista de identificadores.", linha);
    }
}

void lista_ident(FILE *input, Token *token, TabelaSimbolos *tabela) {
    if (identificador(token)) {
        proximo_token(input, token, tabela);
        while (token->tipo == PONT_ && strcmp(token->valor, "5") == 0) { // ,
            proximo_token(input, token, tabela);
            if (identificador(token)) {
                proximo_token(input, token, tabela);
            } else {
                gera_erro("Identificador esperado apos a virgula na lista de identificadores.", linha);
            }
        }
    } else {
        gera_erro("Identificador esperado na lista de identificadores.", linha);
    }
}

void tipo(FILE *input, Token *token, TabelaSimbolos *tabela) {
    if (token->tipo == PC_ && (strcmp(token->valor, "29") == 0 || strcmp(token->valor, "30") == 0)) { // integer ou boolean
        proximo_token(input, token, tabela);
    } else {
        gera_erro("Tipo de variavel esperado apos os dois pontos.", linha);
    }
}

void parte_decl_subrotinas(FILE *input, Token *token, TabelaSimbolos *tabela) {
    while (token->tipo == PC_ && strcmp(token->valor, "2") == 0) { // procedure
        decl_procedimento(input, token, tabela);
        if (token->tipo == PONT_ && strcmp(token->valor, "6") == 0) { // ;
            proximo_token(input, token, tabela);
        }
    }
}

void decl_procedimento(FILE *input, Token *token, TabelaSimbolos *tabela) {
    if (token->tipo == PC_ && strcmp(token->valor, "2") == 0) { // procedure
        proximo_token(input, token, tabela);
        if (identificador(token)) {
            cria_identificador_procedure(token, tabela);
            proximo_token(input, token, tabela);
            if (token->tipo == PONT_ && strcmp(token->valor, "1") == 0) { // (
                parametros_formais(input, token, tabela);
            }
            if (token->tipo == PONT_ && strcmp(token->valor, "6") == 0) { // ;
                proximo_token(input, token, tabela);
                bloco(input, token, tabela);
            } else {
                gera_erro("Ponto e virgula esperado apos declarar procedimento.", linha);
            }
        } else {
            gera_erro("Identificador esperado após 'procedure'.", linha);
        }
    }
}

void parametros_formais(FILE *input, Token *token, TabelaSimbolos *tabela) {
    if (token->tipo == PONT_ && strcmp(token->valor, "1") == 0) { // (
        proximo_token(input, token, tabela);
        secao_parametros_formais(input, token, tabela);
        while (token->tipo == PONT_ && strcmp(token->valor, "6") == 0) { // ;
            proximo_token(input, token, tabela);
            secao_parametros_formais(input, token, tabela);
        }
        if (token->tipo == PONT_ && strcmp(token->valor, "2") == 0) { // )
            proximo_token(input, token, tabela);
        } else {
            gera_erro("Parentese direito esperado.", linha);
        }
    }
}

void secao_parametros_formais(FILE *input, Token *token, TabelaSimbolos *tabela) {
    if (token->tipo == PC_ && strcmp(token->valor, "1") == 0) { // var
        proximo_token(input, token, tabela);
    }
    lista_ident(input, token, tabela);
    if (token->tipo == PONT_ && strcmp(token->valor, "7") == 0) { // :
        proximo_token(input, token, tabela);
        if (identificador(token)) {
            proximo_token(input, token, tabela);
        } else {
            gera_erro("Identificador esperado após ':'.", linha);
        }
    } else {
        gera_erro("Dois pontos esperado apos a lista de identificadores.", linha);
    }
}

void comando_composto(FILE *input, Token *token, TabelaSimbolos *tabela) {
    if (token->tipo == PC_ && strcmp(token->valor, "4") == 0) { // begin
        proximo_token(input, token, tabela);
        comando(input, token, tabela);
        while (token->tipo == PONT_ && strcmp(token->valor, "6") == 0) { // ;
            proximo_token(input, token, tabela);
            comando(input, token, tabela);
        }
        if (token->tipo == PONT_ && strcmp(token->valor, "8") == 0) { // .
            return;  // temos um ponto, retorna a funcao bloco
        } 
    } else {
        gera_erro("'begin' esperado no início do comando composto.", linha);
    }
}

void comando(FILE *input, Token *token, TabelaSimbolos *tabela) {
    printf("Comando: Token { tipo: %d, valor: %s }\n", token->tipo, token->valor);

    if (identificador(token) && !identificador_procedure(token)) {
        atribuicao(input, token, tabela);
    } else if (token->tipo == PC_ && strcmp(token->valor, "6") == 0) { // if
        comando_condicional(input, token, tabela);
    } else if (token->tipo == PC_ && strcmp(token->valor, "8") == 0) { // else
        comando_condicional(input, token, tabela);
    } else if (token->tipo == PC_ && strcmp(token->valor, "9") == 0) { // while
        comando_repetitivo(input, token, tabela);
    } else if (token->tipo == PC_ && strcmp(token->valor, "4") == 0) { // begin
        comando_composto(input, token, tabela);
    } else if (token->tipo == PC_ && strcmp(token->valor, "5") == 0) { // end
        proximo_token(input, token, tabela);
        if (token->tipo == PONT_ && strcmp(token->valor, "8") == 0) { // .
        	return;
		} else if (!(token->tipo == PONT_ && strcmp(token->valor, "6") == 0))  { // ;
        	gera_erro("Ponto e virgula esperado apos 'end'.", linha);
		}
    } else if (token->tipo == PC_ && (strcmp(token->valor, "32") == 0 || strcmp(token->valor, "33") == 0)) { // read ou write
        chamada_procedimento(input, token, tabela);
    } else if (token->tipo == IDENT_PROCED_) {
    	proximo_token(input, token, tabela);
    	if (token->tipo == PONT_ && strcmp(token->valor, "6") == 0) { // ;
            proximo_token(input, token, tabela);
            chamada_procedimento(input, token, tabela);
        } else {
            gera_erro("Ponto e virgula esperado apos identificador de procedure.", linha);
    	
    }
	
	 } else {
        gera_erro("Comando invalido.", linha);
    }
}

void atribuicao(FILE *input, Token *token, TabelaSimbolos *tabela) {
    if (identificador(token)) {
        proximo_token(input, token, tabela);
        if (token->tipo == OPER_ && strcmp(token->valor, "13") == 0) { // :=
            proximo_token(input, token, tabela);
            expressao(input, token, tabela);
        } else {
            gera_erro("Operador ':=' esperado ao atribuir.", linha);
        }
    } else {
        gera_erro("Identificador esperado ao atribuir.", linha);
    }
}

void chamada_procedimento(FILE *input, Token *token, TabelaSimbolos *tabela) {
    if (token->tipo == PC_ && (strcmp(token->valor, "32") == 0 || strcmp(token->valor, "33") == 0)) { // read ou write
        proximo_token(input, token, tabela);
        if (token->tipo == PONT_ && strcmp(token->valor, "1") == 0) { // (
            proximo_token(input, token, tabela);
            if (identificador(token)) {
                proximo_token(input, token, tabela);
                if (token->tipo == PONT_ && strcmp(token->valor, "2") == 0) { // )
                    proximo_token(input, token, tabela);
                } else {
                    gera_erro("Parentese direito esperado.", linha);
                }
            } else {
                gera_erro("Identificador esperado apos '('.", linha);
            }
        } else {
            gera_erro("Parentese esquerdo esperado apos 'read' ou 'write'.", linha);
        }
    }
}

void comando_condicional(FILE *input, Token *token, TabelaSimbolos *tabela) {
    if (token->tipo == PC_ && strcmp(token->valor, "6") == 0) { // if
        proximo_token(input, token, tabela);
        expressao(input, token, tabela);
        if (token->tipo == PC_ && strcmp(token->valor, "7") == 0) { // then
            proximo_token(input, token, tabela);
            comando(input, token, tabela);
            if (token->tipo == PC_ && strcmp(token->valor, "8") == 0) { // else
                proximo_token(input, token, tabela);
                comando(input, token, tabela);
            }
        } else {
            gera_erro("'then' esperado apos a expressao do 'if'.", linha);
        }
    } else if (token->tipo == PC_ && strcmp(token->valor, "8") == 0) { // else
        proximo_token(input, token, tabela);
        comando(input, token, tabela);
    }
}

void comando_repetitivo(FILE *input, Token *token, TabelaSimbolos *tabela) {
    if (token->tipo == PC_ && strcmp(token->valor, "9") == 0) { // while
        proximo_token(input, token, tabela);
        expressao(input, token, tabela);
        if (token->tipo == PC_ && strcmp(token->valor, "10") == 0) { // do
            proximo_token(input, token, tabela);
            comando(input, token, tabela);
        } else {
            gera_erro("'do' esperado apos a expressao do 'while'.", linha);
        }
    }
}

void expressao(FILE *input, Token *token, TabelaSimbolos *tabela) {
    expressao_simples(input, token, tabela);
    if (token->tipo == OPER_ && (strcmp(token->valor, "6") == 0 || strcmp(token->valor, "7") == 0 || strcmp(token->valor, "5") == 0 ||
        strcmp(token->valor, "8") == 0 || strcmp(token->valor, "9") == 0 || strcmp(token->valor, "10") == 0)) {
        relacao(input, token, tabela);
        expressao_simples(input, token, tabela);
    }
}

void relacao(FILE *input, Token *token, TabelaSimbolos *tabela) {
    if (token->tipo == OPER_ && (strcmp(token->valor, "6") == 0 || strcmp(token->valor, "7") == 0 || strcmp(token->valor, "5") == 0 ||
        strcmp(token->valor, "8") == 0 || strcmp(token->valor, "9") == 0 || strcmp(token->valor, "10") == 0)) {
        proximo_token(input, token, tabela);
    } else {
        gera_erro("Operador relacional esperado.", linha);
    }
}

void expressao_simples(FILE *input, Token *token, TabelaSimbolos *tabela) {
    termo(input, token, tabela);
    while (token->tipo == OPER_ && (strcmp(token->valor, "1") == 0 || strcmp(token->valor, "2") == 0 || strcmp(token->valor, "12") == 0)) {
        proximo_token(input, token, tabela);
        termo(input, token, tabela);
    }
}

void termo(FILE *input, Token *token, TabelaSimbolos *tabela) {
    fator(input, token, tabela);
    while ((token->tipo == OPER_ && (strcmp(token->valor, "3") == 0 || strcmp(token->valor, "4") == 0 || strcmp(token->valor, "11") == 0)) || 
            (token->tipo == PC_ && (strcmp(token->valor, "31") == 0))) { // * / and div
        proximo_token(input, token, tabela);
        fator(input, token, tabela);
    }
}

void fator(FILE *input, Token *token, TabelaSimbolos *tabela) {
    if (identificador(token) || verifica_numero(token)) {
        proximo_token(input, token, tabela);
    } else if ((token->tipo == OPER_ && (strcmp(token->valor, "6") == 0 || strcmp(token->valor, "5") == 0 ))) {
        proximo_token(input, token, tabela);
        expressao(input, token, tabela); 
	} else if (token->tipo == PONT_ && strcmp(token->valor, "1") == 0) { // (
        proximo_token(input, token, tabela);
        expressao(input, token, tabela);
        if (token->tipo == PONT_ && strcmp(token->valor, "2") == 0) { // )
            proximo_token(input, token, tabela);
        } else {
            gera_erro("Parentese direito esperado.", linha);
        }
    } else if (token->tipo == OPER_ && strcmp(token->valor, "7") == 0) { // not
        proximo_token(input, token, tabela);
        fator(input, token, tabela);
    } else {
        gera_erro("Fator invalido.", linha);
    }
}

int main() {
    Token token;
    TabelaSimbolos tabela;
    tabela.tamanho = 0;
    inicializa_analise("Output.txt");
    programa(input, &token, &tabela);
    fclose(input);
    return 0;
}
