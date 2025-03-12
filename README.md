# A Simple Compiler Implementation

![Unesp](https://img.shields.io/badge/BCC-UNESP-Bauru.svg)
![License](https://img.shields.io/badge/Code%20License-MIT-blue.svg)

## 📑 Overview

This project implements a compiler for a simplified Pascal language. Developed as part of the Compilers course at UNESP Bauru, this implementation demonstrates fundamental concepts in compiler design through both lexical and syntactic analysis phases.

## 🏗️ Project Components

### 1. Lexical Analyzer

The lexical analyzer (scanner) is responsible for processing the input source code and converting it into tokens. It:

- Identifies keywords (PROGRAM, VAR, PROCEDURE, etc.)
- Recognizes identifiers and numeric literals
- Processes operators (arithmetic, relational, logical, assignment)
- Handles punctuation (parentheses, braces, comma, etc.)
- Reports lexical errors
- Outputs tokens to be consumed by the syntax analyzer

The analyzer generates a token stream with type and value information for each token.

### 2. Syntax Analyzer

The syntax analyzer implements a parser for a simplified Pascal grammar, supporting:

- Program structure validation
- Variable declarations
- Procedure declarations and parameters
- Compound statements
- Control structures (if-then-else, while-do)
- Expression parsing with proper precedence
- Procedure calls

### 3. Symbol Table

The implementation includes a symbol table that:

- Tracks identifiers and their types
- Distinguishes between regular identifiers and procedure identifiers
- Supports lookups for semantic validation

## Implementation Details

### Lexical Analysis

The lexical analyzer scans the source code character by character to:

- Identify token boundaries
- Classify tokens by type (keywords, identifiers, operators, etc.)
- Handle whitespace and comments
- Generate a stream of tokens with their corresponding values

### Parser Structure

The parser follows the recursive descent method, with specific functions for each non-terminal in the grammar:

- `programa`: Entry point for Pascal program
- `bloco`: Handles program blocks
- `parte_decl_var`: Processes variable declarations
- `parte_decl_subrotinas`: Handles procedure declarations
- `comando_composto`: Processes compound statements (BEGIN...END)
- `expressao`, `termo`, `fator`: Handle expression parsing with proper precedence

## 🧰 Getting Started

### Prerequisites

- GCC or compatible C compiler
- Basic understanding of compiler design concepts

## 📋 Project Structure

The implementation consists of several key components:

- **Lexical Analyzer**: Functions to scan source code and generate tokens
- **Token Processing**: Functions to extract and process tokens
- **Symbol Table Management**: Functions to track and verify identifiers
- **Recursive Descent Parser**: Functions that implement the grammar rules
- **Error Handling**: Reporting of lexical and syntax errors with line numbers

## 📊 Example

### Input (Pascal)

```pascal
program Example;
var
  x, y: integer;
begin
  x := 10;
  y := 20;
  write(x + y);
end.
```

### Token Stream (Intermediate output)

```
LINHA
palavra chave valor: 0    // PROGRAM
identificador valor: Example
pontuacao valor: 6        // ;
LINHA
palavra chave valor: 1    // VAR
identificador valor: x
pontuacao valor: 5        // ,
identificador valor: y
pontuacao valor: 7        // :
palavra chave valor: 29   // INTEGER
pontuacao valor: 6        // ;
LINHA
palavra chave valor: 4    // BEGIN
...
```

## 📚 References

- Aho, A. V., Lam, M. S., Sethi, R., & Ullman, J. D. (2006). _Compilers: Principles, Techniques, and Tools_ (2nd ed.). Addison-Wesley.
- Cooper, K. D., & Torczon, L. (2011). _Engineering a Compiler_ (2nd ed.). Morgan Kaufmann.
- Levine, J. R., Mason, T., & Brown, D. (1992). _Lex & Yacc_. O'Reilly Media.

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

<h4 align="center">

<img src="https://socialify.git.ci/luisbernardinello/compilers/image?language=1&name=1&owner=1&pattern=Formal%20Invitation&theme=Auto" alt="Compilers" width="498" height="270" />

</h4>
