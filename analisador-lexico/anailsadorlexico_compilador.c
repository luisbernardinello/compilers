#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAMANHO_PALAVRA 50

char proximo_caractere = ' ';
int linha = 1;
FILE *output, *input;

char *palavras_reservadas[] = {"AND", "ARRAY", "BEGIN", "CASE", "CONST", "DIV", "DO", "DOWNTO", "ELSE", "END", "FILE", "FOR", "FUNCTION", "GOTO",
 "IF", "IN", "INLINE", "LABEL", "MOD", "NIL", "NOT", "OF", "OR", "PACKED", "PROCEDURE", "PROGRAM", "RECORD", "REPEAT", "SET", "THEN", "TO", "TYPE",
  "UNTIL", "VAR", "WHILE", "WITH", NULL};

char *conversao_tokens[][2] = {{"+", "C_SOMA"}, {"-", "C_SUB"}, {"*", "C_MULT"}, {"/", "C_DIV"}, {":=", "C_ATRIB"}, {":", "C_DOIS_PONTOS"}, {";", "C_PONTO_VIRGULA"},
 {",", "C_VIRGULA"}, {".", "C_PONTO"}, {"(", "C_ABRE_PAR"}, {")", "C_FECHA_PAR"}, {"[", "C_ABRE_COL"}, {"]", "C_FECHA_COL"}, {"..", "C_DOIS_PONTOS_PONTO"},
  {"=", "C_IGUAL"}, {"<", "C_MENOR"}, {">", "C_MAIOR"}, {"<=", "C_MENOR_IGUAL"}, {">=", "C_MAIOR_IGUAL"}, {"<>", "C_DIFERENTE"}, {NULL, NULL}};

void PROXIMO(FILE *arquivo) {
    char c;
    c = fgetc(arquivo);
    if (c == '\n') {
        linha++;
        printf("Analise da linha %d feita sem erros.\n", linha - 1);
    }
    if (feof(arquivo)) {
        fclose(output);
        exit(0);
    }
    proximo_caractere = toupper(c);
}

char *CODIGO(char *in, int is_simb) {
    if (is_simb) {
        for (int i = 0; conversao_tokens[i][0] != NULL; i++) {
            if (strcmp(in, conversao_tokens[i][0]) == 0) {
                return conversao_tokens[i][1];
            }
        }
    }
    char *res = (char *)malloc(strlen(in) + 3); // +3 para "C_" e '\0'
    strcpy(res, "C_");
    strcat(res, in);
    return res;
}

void ERRO(int flag) {
    printf("\n\nErro na linha %d: ", linha);
    switch(flag) {
        case 1:
            printf("Identificador ou número mal formatado\n");
            break;
        case 2:
            printf("Símbolo especial desconhecido\n");
            break;
    }
    fclose(output);
    exit(0);
}

int main() {
    input = fopen("Trab1_Compiladores.txt", "r");
    output = fopen("Output.txt", "w");
    if (input == NULL || output == NULL) {
        printf("Erro ao abrir o arquivo\n");
        return 1;
    }

    PROXIMO(input); // Primeira chamada para inicializar o proximo_caractere

    fprintf(output, "%d ", linha);
    
    while (!feof(input)) {
        // Ignorar espaços em branco
        while ((proximo_caractere == ' ' || proximo_caractere == '\t' || proximo_caractere == '\n')) {
            PROXIMO(input);
        }

        // Fim do arquivo
        if (feof(input)) {
            fclose(output);
            return 0;
        }

        // Simbolos especiais
        char s[3] = {proximo_caractere, '\0'}; // Tamanho 3 para "C_" e '\0'
        if (strchr("+-*/;(),.[]:=<>\n", proximo_caractere) != NULL) {
            PROXIMO(input);
            if (strcmp(s, ":") == 0) {
                if (proximo_caractere == '=') {
                    strcat(s, "=");
                    PROXIMO(input);
                }
            }
            fprintf(output, "%s ", CODIGO(s, 1));
        }
        // Palavra reservada ou identificador
        else if (isalpha(proximo_caractere)) {
            char elemento[TAMANHO_PALAVRA] = {0};
            int i = 0;
            do {
                elemento[i++] = proximo_caractere;
                PROXIMO(input);
            } while (isalnum(proximo_caractere) && !feof(input));
            
            // Verifica se se trata de uma palavra reservada
            char *reservada = NULL;
            for (int j = 0; palavras_reservadas[j] != NULL; j++) {
                if (strcmp(elemento, palavras_reservadas[j]) == 0) {
                    reservada = elemento;
                    break;
                }
            }
            if (reservada != NULL) {
                fprintf(output, "%s ", CODIGO(reservada, 0));
            } else {
                fprintf(output, "ID_%s ", elemento);
            }
        }
        // Numero
        else if (isdigit(proximo_caractere)) {
            char elemento[TAMANHO_PALAVRA] = {0};
            int i = 0;
            do {
                elemento[i++] = proximo_caractere;
                PROXIMO(input);
            } while (isdigit(proximo_caractere) && !feof(input));
            
            // Se o próximo caractere for uma letra, gera erro
            if (isalpha(proximo_caractere)) {
                ERRO(1);
            }
            fprintf(output, "NUM_%s ", elemento);
        }
        else {
            if (!feof(input)) {
                ERRO(2);
            }
        }
        if (proximo_caractere == '\n') {
            fprintf(output, "\n%d ", linha);
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}
