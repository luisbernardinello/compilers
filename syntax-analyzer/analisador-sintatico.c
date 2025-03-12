 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 
 /* 
 Tokens in Pascal:
 - PC_: Keywords (0-31, e.g., PROGRAM=0, VAR=1, etc.)
 - PONT_: Punctuation (1-8, e.g., left parenthesis=1, right parenthesis=2, etc.)
 - OPER_: Operators (1-13, e.g., addition=1, subtraction=2, etc.)
 - NUM_: Numbers
 - IDENT_: Identifiers
 - IDENT_PROCED_: Procedure identifiers
 */
 
 int linha = 0;
 FILE *input;
 
 // Token types
 enum TipoToken {
     OPER_,
     PONT_,
     PC_,
     IDENT_,
     NUM_,
     IDENT_PROCED_
 };
 
 // Token structure
 typedef struct {
     enum TipoToken tipo;
     char valor[50];
 } Token;
 
 // Symbol table structure
 typedef struct {
     char identificadores[100][50]; // Symbol table limit: 100 identifiers
     int tipo[100]; 
     int tamanho;
 } TabelaSimbolos;
 
 // Function declarations
 void inicializa_analise(char *nome_arq);
 void extrai_valor_token(FILE *arquivo, Token *token);
 void gera_erro(const char *desc_erro, int linha);
 void proximo_token(FILE *input, Token *token, TabelaSimbolos *tabela);
 bool verifica_numero(Token *token);
 bool identificador(Token *token);
 bool identificador_procedure(Token *token);
 void cria_identificador_procedure(Token *token, TabelaSimbolos *tabela);
 bool verificar_identificador(TabelaSimbolos *tabela, char *identificador, enum TipoToken *tipo);
 void verificar_token_esperado(FILE *input, Token *token, TabelaSimbolos *tabela, 
                             enum TipoToken tipo, const char *valor, const char *mensagem_erro);
 
 // Pascal grammar functions
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
 void comando_vazio(FILE *input, Token *token, TabelaSimbolos *tabela);
 
 // Initialize analysis by opening input file
 void inicializa_analise(char *nome_arq) {
     input = fopen(nome_arq, "r");
 
     if (input == NULL) {
         printf("Nao foi possivel abrir o arquivo de entrada: %s\n", nome_arq);
         exit(1);
     }
 }
 
 // Extract token value from file
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
         } else if (strstr(buffer, "numero") != NULL) {
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
     
     // End of file or token not found
     token->tipo = -1;
     strcpy(token->valor, "EOF");
 }
 
 // Error generation with line number
 void gera_erro(const char *desc_erro, int linha) {
     fprintf(stderr, "Erro sintatico na linha %d: %s\n", linha, desc_erro);
     exit(1);
 }
 
 // Check if token is a number
 bool verifica_numero(Token *token) {
     return token->tipo == NUM_;
 }
 
 // Check if token is an identifier
 bool identificador(Token *token) {
     return token->tipo == IDENT_;
 }
 
 // Check if token is a procedure identifier
 bool identificador_procedure(Token *token) {
     return token->tipo == IDENT_PROCED_;
 }
 
 // Create a procedure identifier in the symbol table
 void cria_identificador_procedure(Token *token, TabelaSimbolos *tabela) {
     for (int i = 0; i < tabela->tamanho; i++) {
         if (strcmp(tabela->identificadores[i], token->valor) == 0) {
             tabela->tipo[i] = IDENT_PROCED_;
             token->tipo = IDENT_PROCED_;
             return;
         }
     }
     
     // If not in table, insert as procedure identifier
     if (tabela->tamanho < 100) {  // Check for table overflow
         strcpy(tabela->identificadores[tabela->tamanho], token->valor);
         tabela->tipo[tabela->tamanho] = IDENT_PROCED_;
         tabela->tamanho++;
         token->tipo = IDENT_PROCED_;
     } else {
         gera_erro("Tabela de simbolos cheia", linha);
     }
 }
 
 // Check if identifier exists in symbol table
 bool verificar_identificador(TabelaSimbolos *tabela, char *identificador, enum TipoToken *tipo) {
     for (int i = 0; i < tabela->tamanho; i++) {
         if (strcmp(tabela->identificadores[i], identificador) == 0) {
             *tipo = tabela->tipo[i];
             return true; // Identifier found
         }
     }
     return false;
 }
 
 // Helper function to check expected tokens
 void verificar_token_esperado(FILE *input, Token *token, TabelaSimbolos *tabela, 
                             enum TipoToken tipo, const char *valor, const char *mensagem_erro) {
     if (token->tipo == tipo && strcmp(token->valor, valor) == 0) {
         proximo_token(input, token, tabela);
     } else {
         gera_erro(mensagem_erro, linha);
     }
 }
 
 // Get next token
 void proximo_token(FILE *input, Token *token, TabelaSimbolos *tabela) {
     extrai_valor_token(input, token);
 
     // Check for EOF
     if (token->tipo == -1) {
         return;
     }
 
     printf("Linha %d: Token { tipo: %d, valor: %s }\n", linha, token->tipo, token->valor);
 
     // Check if token is an identifier
     if (token->tipo == IDENT_) {
         enum TipoToken tipo;
         // Check if identifier is in symbol table
         if (verificar_identificador(tabela, token->valor, &tipo)) {
             token->tipo = tipo; // If in table, update token type
         } else {
             if (tabela->tamanho < 100) {  // Check for table overflow
                 strcpy(tabela->identificadores[tabela->tamanho], token->valor); // If not, insert
                 tabela->tipo[tabela->tamanho] = IDENT_;
                 tabela->tamanho++;
             } else {
                 gera_erro("Tabela de simbolos cheia", linha);
             }
         }
     }
 }
 
 // Program structure (entry point of syntax analysis)
 void programa(FILE *input, Token *token, TabelaSimbolos *tabela) {
     proximo_token(input, token, tabela);
     
     // Check for 'program' keyword
     if (token->tipo == PC_ && strcmp(token->valor, "0") == 0) {
         proximo_token(input, token, tabela);
         
         // Check for program identifier
         if (identificador(token)) {
             proximo_token(input, token, tabela);
             
             // Check for semicolon
             if (token->tipo == PONT_ && strcmp(token->valor, "6") == 0) {
                 proximo_token(input, token, tabela);
                 
                 // Process program block
                 bloco(input, token, tabela);
                 
                 // Check for program end (period)
                 if (token->tipo == PONT_ && strcmp(token->valor, "8") == 0) {
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
 
 // Block structure (can contain variable declarations, procedures, and commands)
 void bloco(FILE *input, Token *token, TabelaSimbolos *tabela) {
     // Variable declarations part (optional)
     if (token->tipo == PC_ && strcmp(token->valor, "1") == 0) { // var
         parte_decl_var(input, token, tabela);
     }
     
     // Subroutine declarations part (optional)
     if (token->tipo == PC_ && strcmp(token->valor, "2") == 0) { // procedure
         parte_decl_subrotinas(input, token, tabela);
     }
     
     // Compound command part (required)
     if (token->tipo == PC_ && strcmp(token->valor, "4") == 0) { // begin
         comando_composto(input, token, tabela);
     } else if (token->tipo == PONT_ && strcmp(token->valor, "8") == 0) { // .
         return; // We have a period, return to main function
     } else {
         gera_erro("'begin' esperado para iniciar um bloco.", linha);
     }
 }
 
 // Variable declarations part
 void parte_decl_var(FILE *input, Token *token, TabelaSimbolos *tabela) {
     if (token->tipo == PC_ && strcmp(token->valor, "1") == 0) { // var
         proximo_token(input, token, tabela);
         decl_var(input, token, tabela);
         
         // Process multiple variable declarations
         while (token->tipo == PONT_ && strcmp(token->valor, "6") == 0) { // ;
             proximo_token(input, token, tabela);
             
             // Check if we're done with variable declarations
             if (token->tipo == PC_ && (strcmp(token->valor, "2") == 0 || strcmp(token->valor, "4") == 0)) { // procedure or begin
                 return;
             }
             
             // Process next variable declaration
             decl_var(input, token, tabela);
         }
     }
 }
 
 // Variable declaration
 void decl_var(FILE *input, Token *token, TabelaSimbolos *tabela) {
     lista_ident(input, token, tabela);
     
     // Check for type separator
     if (token->tipo == PONT_ && strcmp(token->valor, "7") == 0) { // :
         proximo_token(input, token, tabela);
         tipo(input, token, tabela);
     } else {
         gera_erro("Dois pontos esperado apos a lista de identificadores.", linha);
     }
 }
 
 // Identifier list
 void lista_ident(FILE *input, Token *token, TabelaSimbolos *tabela) {
     if (identificador(token)) {
         proximo_token(input, token, tabela);
         
         // Process multiple identifiers separated by commas
         while (token->tipo == PONT_ && strcmp(token->valor, "5") == 0) { // ,
             proximo_token(input, token, tabela);
             
             // Each comma must be followed by an identifier
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
 
 // Type declaration
 void tipo(FILE *input, Token *token, TabelaSimbolos *tabela) {
     if (token->tipo == PC_ && (strcmp(token->valor, "29") == 0 || strcmp(token->valor, "30") == 0)) { // integer or boolean
         proximo_token(input, token, tabela);
     } else {
         gera_erro("Tipo de variavel esperado apos os dois pontos.", linha);
     }
 }
 
 // Subroutine declarations part
 void parte_decl_subrotinas(FILE *input, Token *token, TabelaSimbolos *tabela) {
     // Process multiple procedure declarations
     while (token->tipo == PC_ && strcmp(token->valor, "2") == 0) { // procedure
         decl_procedimento(input, token, tabela);
         
         // Each procedure must end with a semicolon
         if (token->tipo == PONT_ && strcmp(token->valor, "6") == 0) { // ;
             proximo_token(input, token, tabela);
         } else {
             gera_erro("Ponto e virgula esperado apos declaracao de procedimento.", linha);
         }
     }
 }
 
 // Procedure declaration
 void decl_procedimento(FILE *input, Token *token, TabelaSimbolos *tabela) {
     if (token->tipo == PC_ && strcmp(token->valor, "2") == 0) { // procedure
         proximo_token(input, token, tabela);
         
         // Check for procedure identifier
         if (identificador(token)) {
             cria_identificador_procedure(token, tabela);
             proximo_token(input, token, tabela);
             
             // Check for formal parameters (optional)
             if (token->tipo == PONT_ && strcmp(token->valor, "1") == 0) { // (
                 parametros_formais(input, token, tabela);
             }
             
             // Procedure header must end with semicolon
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
 
 // Formal parameters
 void parametros_formais(FILE *input, Token *token, TabelaSimbolos *tabela) {
     if (token->tipo == PONT_ && strcmp(token->valor, "1") == 0) { // (
         proximo_token(input, token, tabela);
         secao_parametros_formais(input, token, tabela);
         
         // Process multiple parameter sections
         while (token->tipo == PONT_ && strcmp(token->valor, "6") == 0) { // ;
             proximo_token(input, token, tabela);
             secao_parametros_formais(input, token, tabela);
         }
         
         // Parameter list must end with right parenthesis
         if (token->tipo == PONT_ && strcmp(token->valor, "2") == 0) { // )
             proximo_token(input, token, tabela);
         } else {
             gera_erro("Parentese direito esperado.", linha);
         }
     }
 }
 
 // Formal parameters section
 void secao_parametros_formais(FILE *input, Token *token, TabelaSimbolos *tabela) {
     // VAR parameter (reference parameter, optional)
     if (token->tipo == PC_ && strcmp(token->valor, "1") == 0) { // var
         proximo_token(input, token, tabela);
     }
     
     // Parameter identifiers
     lista_ident(input, token, tabela);
     
     // Parameter type
     if (token->tipo == PONT_ && strcmp(token->valor, "7") == 0) { // :
         proximo_token(input, token, tabela);
         
         // Parameter type must be an identifier
         if (identificador(token)) {
             proximo_token(input, token, tabela);
         } else {
             gera_erro("Identificador esperado após ':'.", linha);
         }
     } else {
         gera_erro("Dois pontos esperado apos a lista de identificadores.", linha);
     }
 }
 
 // Compound command
 void comando_composto(FILE *input, Token *token, TabelaSimbolos *tabela) {
     if (token->tipo == PC_ && strcmp(token->valor, "4") == 0) { // begin
         proximo_token(input, token, tabela);
         
         // Process first command
         comando(input, token, tabela);
         
         // Process multiple commands separated by semicolons
         while (token->tipo == PONT_ && strcmp(token->valor, "6") == 0) { // ;
             proximo_token(input, token, tabela);
             
             // Check for end of compound command
             if (token->tipo == PC_ && strcmp(token->valor, "5") == 0) { // end
                 break;
             }
             
             // Process next command
             comando(input, token, tabela);
         }
         
         // Check for end keyword
         if (token->tipo == PC_ && strcmp(token->valor, "5") == 0) { // end
             proximo_token(input, token, tabela);
         } else {
             gera_erro("'end' esperado no final do comando composto.", linha);
         }
     } else {
         gera_erro("'begin' esperado no início do comando composto.", linha);
     }
 }
 
 // Command
 void comando(FILE *input, Token *token, TabelaSimbolos *tabela) {
     printf("Comando: Token { tipo: %d, valor: %s }\n", token->tipo, token->valor);
 
     // Empty command (no operation)
     if (token->tipo == PC_ && strcmp(token->valor, "5") == 0) { // end
         return; // Empty command, just return
     }
     
     // Assignment command
     else if (identificador(token) && !identificador_procedure(token)) {
         atribuicao(input, token, tabela);
     }
     
     // Conditional command (if-then-else)
     else if (token->tipo == PC_ && strcmp(token->valor, "6") == 0) { // if
         comando_condicional(input, token, tabela);
     }
     
     // Repetitive command (while-do)
     else if (token->tipo == PC_ && strcmp(token->valor, "9") == 0) { // while
         comando_repetitivo(input, token, tabela);
     }
     
     // Compound command (begin-end)
     else if (token->tipo == PC_ && strcmp(token->valor, "4") == 0) { // begin
         comando_composto(input, token, tabela);
     }
     
     // Read or Write procedure call
     else if (token->tipo == PC_ && (strcmp(token->valor, "32") == 0 || strcmp(token->valor, "33") == 0)) { // read or write
         chamada_procedimento(input, token, tabela);
     }
     
     // User-defined procedure call
     else if (token->tipo == IDENT_PROCED_) {
         // Store procedure identifier for error reporting
         char proc_name[50];
         strcpy(proc_name, token->valor);
         
         proximo_token(input, token, tabela);
         
         // Handle procedure parameters if present
         if (token->tipo == PONT_ && strcmp(token->valor, "1") == 0) { // (
             parametros_formais(input, token, tabela);
         }
     }
     
     // Invalid command
     else {
         gera_erro("Comando invalido.", linha);
     }
 }
 
 // Empty command
 void comando_vazio(FILE *input, Token *token, TabelaSimbolos *tabela) {
     // Do nothing, represents null command
     return;
 }
 
 // Assignment command
 void atribuicao(FILE *input, Token *token, TabelaSimbolos *tabela) {
     if (identificador(token)) {
         // Store variable name for error reporting
         char var_name[50];
         strcpy(var_name, token->valor);
         
         proximo_token(input, token, tabela);
         
         // Check for assignment operator
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
 
 // Procedure call
 void chamada_procedimento(FILE *input, Token *token, TabelaSimbolos *tabela) {
     // Read or Write procedure
     if (token->tipo == PC_ && (strcmp(token->valor, "32") == 0 || strcmp(token->valor, "33") == 0)) { // read or write
         char proc_name[50];
         strcpy(proc_name, token->valor);
         
         proximo_token(input, token, tabela);
         
         // Check for parameter list
         if (token->tipo == PONT_ && strcmp(token->valor, "1") == 0) { // (
             proximo_token(input, token, tabela);
             
             // Check for parameter
             if (identificador(token)) {
                 proximo_token(input, token, tabela);
                 
                 // Check for closing parenthesis
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
     // User-defined procedure call (already handled in comando function)
 }
 
 // Conditional command (if-then-else)
 void comando_condicional(FILE *input, Token *token, TabelaSimbolos *tabela) {
     if (token->tipo == PC_ && strcmp(token->valor, "6") == 0) { // if
         proximo_token(input, token, tabela);
         
         // Condition expression
         expressao(input, token, tabela);
         
         // Then part
         if (token->tipo == PC_ && strcmp(token->valor, "7") == 0) { // then
             proximo_token(input, token, tabela);
             comando(input, token, tabela);
             
             // Else part (optional)
             if (token->tipo == PC_ && strcmp(token->valor, "8") == 0) { // else
                 proximo_token(input, token, tabela);
                 comando(input, token, tabela);
             }
         } else {
             gera_erro("'then' esperado apos a expressao do 'if'.", linha);
         }
     }
 }
 
 // Repetitive command (while-do)
 void comando_repetitivo(FILE *input, Token *token, TabelaSimbolos *tabela) {
     if (token->tipo == PC_ && strcmp(token->valor, "9") == 0) { // while
         proximo_token(input, token, tabela);
         
         // Condition expression
         expressao(input, token, tabela);
         
         // Do part
         if (token->tipo == PC_ && strcmp(token->valor, "10") == 0) { // do
             proximo_token(input, token, tabela);
             comando(input, token, tabela);
         } else {
             gera_erro("'do' esperado apos a expressao do 'while'.", linha);
         }
     }
 }
 
 // Expression
 void expressao(FILE *input, Token *token, TabelaSimbolos *tabela) {
     // First simple expression
     expressao_simples(input, token, tabela);
     
     // Relation operator and second simple expression (optional)
     if (token->tipo == OPER_ && (
         strcmp(token->valor, "6") == 0 ||  // =
         strcmp(token->valor, "7") == 0 ||  // <>
         strcmp(token->valor, "5") == 0 ||  // <
         strcmp(token->valor, "8") == 0 ||  // <=
         strcmp(token->valor, "9") == 0 ||  // >
         strcmp(token->valor, "10") == 0))  // >=
     {
         relacao(input, token, tabela);
         expressao_simples(input, token, tabela);
     }
 }
 
 // Relation operator
 void relacao(FILE *input, Token *token, TabelaSimbolos *tabela) {
     if (token->tipo == OPER_ && (
         strcmp(token->valor, "6") == 0 ||  // =
         strcmp(token->valor, "7") == 0 ||  // <>
         strcmp(token->valor, "5") == 0 ||  // <
         strcmp(token->valor, "8") == 0 ||  // <=
         strcmp(token->valor, "9") == 0 ||  // >
         strcmp(token->valor, "10") == 0))  // >=
     {
         proximo_token(input, token, tabela);
     } else {
         gera_erro("Operador relacional esperado.", linha);
     }
 }
 
 // Simple expression
 void expressao_simples(FILE *input, Token *token, TabelaSimbolos *tabela) {
     // Optional sign
     if (token->tipo == OPER_ && (strcmp(token->valor, "1") == 0 || strcmp(token->valor, "2") == 0)) { // + or -
         proximo_token(input, token, tabela);
     }
     
     // First term
     termo(input, token, tabela);
     
     // Additional terms with additive operators
     while (token->tipo == OPER_ && (
         strcmp(token->valor, "1") == 0 ||  // +
         strcmp(token->valor, "2") == 0 ||  // -
         strcmp(token->valor, "12") == 0))  // or
     {
         proximo_token(input, token, tabela);
         termo(input, token, tabela);
     }
 }
 
 // Term
 void termo(FILE *input, Token *token, TabelaSimbolos *tabela) {
     // First factor
     fator(input, token, tabela);
     
     // Additional factors with multiplicative operators
     while ((token->tipo == OPER_ && (
             strcmp(token->valor, "3") == 0 ||  // *
             strcmp(token->valor, "4") == 0 ||  // /
             strcmp(token->valor, "11") == 0))  // and
         || (token->tipo == PC_ && (strcmp(token->valor, "31") == 0)))  // div
     {
         proximo_token(input, token, tabela);
         fator(input, token, tabela);
     }
 }
 
 // Factor
 void fator(FILE *input, Token *token, TabelaSimbolos *tabela) {
     // Identifier or Number
     if (identificador(token) || verifica_numero(token)) {
         proximo_token(input, token, tabela);
     }
     // Parenthesized expression
     else if (token->tipo == PONT_ && strcmp(token->valor, "1") == 0) { // (
         proximo_token(input, token, tabela);
         expressao(input, token, tabela);
         
         if (token->tipo == PONT_ && strcmp(token->valor, "2") == 0) { // )
             proximo_token(input, token, tabela);
         } else {
             gera_erro("Parentese direito esperado.", linha);
         }
     }
     // Negation (not)
     else if (token->tipo == OPER_ && strcmp(token->valor, "7") == 0) { // not
         proximo_token(input, token, tabela);
         fator(input, token, tabela);
     }
     // Invalid factor
     else {
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