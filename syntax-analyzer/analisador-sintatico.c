#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define TAMANHO_PALAVRA 50

int linha = 0;
FILE *input;

// tipos de token
enum TipoToken {
    OPER_,
    PONT_,
    PC_,
    IDENT_,
    NUM_
};

// struct para o token
typedef struct {
    enum TipoToken tipo;
    int valor;
} Token;


void inicializa_analise(char *nome_arq) {
    input = fopen(nome_arq, "r");

    if (input == NULL) {
        printf("nao foi possivel abrir o arquivo de entrada: %s\n", nome_arq);
        printf("\n aperte uma tecla para o encerramento.");
        fflush(stdin);
        getchar();
        exit(1);
    }
}

// extrai o valor do token e define o tipo
void extrai_valor_token(FILE *arquivo, Token *token) {
    char buffer[1024]; // buffer armazena as linhas
    char *posicao; // ponteiro que armazena a posição da string "valor:"

    while (fgets(buffer, sizeof(buffer), arquivo) != NULL) {
        // verifica se a linha atual contém a string "LINHA"
        if (strstr(buffer, "LINHA") != NULL) {
            linha++;
            continue;
        }

        // verifica o tipo de token baseado nas palavras-chave
        if (strstr(buffer, "palavra chave") != NULL) {
            token->tipo = PC_;
        } else if (strstr(buffer, "identificador") != NULL) {
            token->tipo = IDENT_;
        } else if (strstr(buffer, "numero") != NULL) {
            token->tipo = NUM_;
        } else if (strstr(buffer, "operador") != NULL) {
            token->tipo = OPER_;
        } else if (strstr(buffer, "pontuacao") != NULL) {
            token->tipo = PONT_;
        } else {
            continue;
        }

        // procura por "valor:" na linha
        posicao = strstr(buffer, "valor:");
        if (posicao != NULL) {
            // se encontrou "valor:" na linha pega o valor depois de "valor: "
            posicao += strlen("valor: ");

            // encontra o proximo token (até a quebra de linha)
            char *valor_str = strtok(posicao, "\n");
            if (valor_str != NULL) {
                token->valor = atoi(valor_str);
                printf("Linha %d: Token (Tipo = %d, Valor = %d) sem erros sintaticos.\n", linha, token->tipo, token->valor); // printa linha e token
                return;
            }
        }
    }
}

// gera o erro, apontando a linha do erro
void gera_erro(const char *desc_erro, int *linha) {
    fprintf(stderr, "Ocorreu um erro: ->'%s'<- \n Erro gerado na linha %d.\n", desc_erro, *linha);
    exit(1);
}

bool verifica_numero(Token *token) {
    return token->tipo == NUM_;
}

// booleano para verificar se se trata de um identificador
bool identificador(Token *token) {
    return token->tipo == IDENT_;
}

// booleano para verificar se é parenteses (contador para saber se a mesma quantidade de abre é a de fecha, sem usar pilha)
bool verifica_parenteses(FILE *arquivo, Token *token) {
    int contador = 0;
    while (true) {
        extrai_valor_token(arquivo, token);
        if (token->valor == 1 && token->tipo == PONT_) { // (
            contador++;
        } else if (token->valor == 2 && token->tipo == PONT_) { // )
            contador--;
            if (contador == 0) {
                return true;
            }
        }
        
        
    }
    return contador == 0;
}

// função de inicio da analise sintatica, verifica se o token inicial é o comando PROGRAM.
void simbolo_inicial(FILE *input, Token *token, int *linha) {
    extrai_valor_token(input, token);
    if (token->valor == 0 && token->tipo == PC_) {  // token PROGRAM
        extrai_valor_token(input, token);
        if (identificador(token)) {
            bloco(input, token, linha);
        } 
        
    } else {
        gera_erro("Comando PROGRAM ausente", *linha);
    }
}
// analise bloco
void bloco(FILE *input, Token *token, int *linha) {
    extrai_valor_token(input, token);
    declara_variaveis(input, token, linha);
    declara_subrotinas(input, token, linha);
    analise_comando_composto(input, token, linha);
    return;
}
// analisa a declaracao das variaveis
void analise_variaveis(FILE *input, Token *token, int *linha) {
    lista_de_identificadores(input, token, linha);
    if (token->valor == 7 && token->tipo == PONT_) {
        extrai_valor_token(input, token);
        if ((token->valor == 29 && token->tipo == PC_) || (token->valor == 30 && token->tipo == PC_))
            return;
        else
            gera_erro("Variavel declarada de forma incorreta", linha);
    } else
        gera_erro("Variavel com tipo incorreto ", linha);
    return;
}

// declara variaveis
void declara_variaveis(FILE *input, Token *token, int *linha) {
    if (token->valor == 1 && token->tipo == PC_) {
        analise_variaveis(input, token, linha);
        extrai_valor_token(input, token);
        if (token->valor == 6 && token->tipo == PONT_) {
            extrai_valor_token(input, token);
            return;
        } else
            gera_erro("Ponto e virgula ausente", linha);
    }
    return;
}
// analisa lista de indicadores
void lista_de_identificadores(FILE *input, Token *token, int *linha) {
    extrai_valor_token(input, token);
    if (identificador(token)) {
        extrai_valor_token(input, token);
        if (token->valor == 5 && token->tipo == PONT_)
            lista_de_identificadores(input, token, linha);
        else
            return;
    } else
        gera_erro("Identificador ausente", linha);
}
// declara subrotinas (procedures)
void declara_subrotinas(FILE *input, Token *token, int *linha) {
    if (token->valor == 2 && token->tipo == PC_) {
        analise_subrotinas(input, token, linha);
        return;
    }
    return;
}


void analise_subrotinas(FILE *input, Token *token, int *linha) {
    extrai_valor_token(input, token);
    if (identificador(token)) {
        analise_parametros_formais(input, token, linha);
    } else
        gera_erro("Identificador ausente", linha);
    bloco(input, token, linha);
}

// analisa os parametros formais de uma procedure
void analise_parametros_formais(FILE *input, Token *token, int *linha) {
    extrai_valor_token(input, token);
    if (token->valor == 6 && token->tipo == PONT_)
        return;
}

void analise_comando_sem_label(FILE *input, Token *token, int *linha) {
    if ((token->valor < 0 && token->tipo == IDENT_) || (token->valor == 31 && token->tipo == PC_) || (token->valor == 32 && token->tipo == PC_)) {
        extrai_valor_token(input, token);
        if (token->valor == 13 && token->tipo == OPER_)
            analise_exp_atrib(input, token, linha);
        else
            analise_chamada_subrotina(input, token, linha);
        return;
    } else if (token->valor == 4 && token->tipo == PC_) { //BEGIN
        analise_comando_composto(input, token, linha);
    } else if (token->valor == 6 && token->tipo == PC_) { //IF
        analise_comando_condicional(input, token, linha);
    } else if (token->valor == 9 && token->tipo == PC_) { //WHILE
        analise_comando_repeticao(input, token, linha);
    } else {
        printf("Erro: token não reconhecido durante a analise sintatica. Tipo = %d, Valor = %d, Linha = %d\n", token->tipo, token->valor, linha);
    }
}
// analise de comando rotulado
void analise_comando_com_label(FILE *input, Token *token, int *linha) {
    if (token->tipo == NUM_)
        extrai_valor_token(input, token);
    analise_comando_sem_label(input, token, linha);
    return;
}
// analise de comando composto
void analise_comando_composto(FILE *input, Token *token, int *linha) {
    if (token->valor == 4 && token->tipo == PC_) { //begin
        extrai_valor_token(input, token);
        while (token->valor != 5 || token->tipo != PC_) { //end
            analise_comando_com_label(input, token, linha);
        }
        extrai_valor_token(input, token);
        if ((token->valor == 6 && token->tipo == PONT_) || (token->valor == 8 && token->tipo == PONT_)) {
            extrai_valor_token(input, token);
            return;
        } else
            gera_erro("Ponto e virgula ausente", linha);
    } else {
        gera_erro("Comando BEGIN ausente", linha);
    }
    return;
}
// analise comando de repetição
void analise_comando_repeticao(FILE *input, Token *token, int *linha) {
    extrai_valor_token(input, token);
    analise_exp_completa(input, token, linha);
    if (token->valor == 10 && token->tipo == PC_) {//do
        extrai_valor_token(input, token);
        analise_comando_sem_label(input, token, linha);
        return;
    } else
        gera_erro("Comando DO ausente", linha);
    return;
}
// analise de comando condicional
void analise_comando_condicional(FILE *input, Token *token, int *linha) {
    extrai_valor_token(input, token);
    if (token->valor == 1 && token->tipo == PONT_) { // (
        if (!verifica_parenteses(input, token)) {
            gera_erro("Parenteses nao correspondem na expressao condicional", linha);
        }
    } else {
        analise_exp_completa(input, token, linha);
    }
    
    if (token->valor == 7 && token->tipo == PC_) { // THEN
        extrai_valor_token(input, token);
        analise_comando_sem_label(input, token, linha);
        extrai_valor_token(input, token);
        if (token->valor == 8 && token->tipo == PC_) { // ELSE
            extrai_valor_token(input, token);
            analise_comando_sem_label(input, token, linha);
            if (token->valor == 6 && token->tipo == PONT_) { // ;
                extrai_valor_token(input, token);
                return;
            } else {
                gera_erro("Ponto e virgula ausente", linha);
            }
        } else {
            return;
        }
    } else {
        gera_erro("Comando THEN ausente", linha);
    }
}

void analisa_fator_exp(FILE *input, Token *token, int *linha) {
    if (identificador(token)) {
        extrai_valor_token(input, token);
        return;
    } else if (verifica_numero(token)) {
        extrai_valor_token(input, token);
        return;
    }
    return;
}

void analisa_termo_exp(FILE *input, Token *token, int *linha) {
    analisa_fator_exp(input, token, linha);
    if ((token->valor == 3 && token->tipo == OPER_) || (token->valor == 4 && token->tipo == OPER_)) { //mult e div
        extrai_valor_token(input, token);
        analisa_fator_exp(input, token, linha);
    }
    return;
}

void analisa_exp_simples(FILE *input, Token *token, int *linha) {
    if ((token->valor == 1 && token->tipo == OPER_) || (token->valor == 2 && token->tipo == OPER_)) //soma e subt
        extrai_valor_token(input, token);
    analisa_termo_exp(input, token, linha);

    if ((token->valor == 1 && token->tipo == OPER_) || (token->valor == 2 && token->tipo == OPER_) || (token->valor == 3 && token->tipo == OPER_) || (token->valor == 4 && token->tipo == OPER_) || (token->valor == 6 && token->tipo == OPER_)) {
        extrai_valor_token(input, token);
        analisa_termo_exp(input, token, linha);
    }
    return;
}

int verifica_operador_relacional(Token *token) {
    if ((token->valor == 6 && token->tipo == OPER_) || (token->valor == 5 && token->tipo == OPER_) || (token->valor == 9 && token->tipo == OPER_))
        return 1;
    else
        return 0;
}

void analise_exp_completa(FILE *input, Token *token, int *linha) {
    analisa_exp_simples(input, token, linha);
    if (verifica_operador_relacional(token)) {
        extrai_valor_token(input, token);
        analisa_exp_simples(input, token, linha);
    }
    return;
}

void analise_exp_atrib(FILE *input, Token *token, int *linha) {
    extrai_valor_token(input, token);
    analise_exp_completa(input, token, linha);
    if (token->valor == 6 && token->tipo == PONT_) {
        extrai_valor_token(input, token);
        return;
    } else
        gera_erro("Ponto e virgula ausente", linha);
    return;
}

void analise_lista_exp(FILE *input, Token *token, int *linha) {
    analise_exp_completa(input, token, linha);
    if ((token->valor == 5 && token->tipo == PONT_)) {
        extrai_valor_token(input, token);
        analise_lista_exp(input, token, linha);
    } else if (token->valor == 6 && token->tipo == PONT_) {
        extrai_valor_token(input, token);
        return;
    } else
        gera_erro("Ponto e virgula ausente", linha);
    return;
}

void analise_chamada_subrotina(FILE *input, Token *token, int *linha) {
    analise_lista_exp(input, token, linha);
    return;
}


int main() {
    //inicializa_analise("C:/ComputerWizard/BCC/disciplina-compiladores/lexical-analyzer/Output.txt");
    inicializa_analise("C:/ComputerWizard/BCC/disciplina-compiladores/lexical-analyzer/Output-erro.txt");
   
    Token token;
    // função de inicio da analise sintatica pelo simbolo inicio do output da analise lexica
    simbolo_inicial(input, &token, &linha);

    fclose(input);

    fflush(stdin);
    getchar();
    return 0;
}
