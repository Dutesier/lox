# Lox

Welcome to the **Lox Interpreter** project! This repository contains an implementation of the Lox programming language interpreter, developed as an exercise based on the book [*Crafting Interpreters* by Robert Nystrom](http://www.craftinginterpreters.com/). 

This project follows the concepts and structure outlined in the book, providing an in-depth exploration of language design and interpreter implementation.

## Overview

Lox is a dynamically-typed, interpreted language designed to be simple yet powerful enough to demonstrate key programming language concepts. This interpreter implements the Lox language by walking through the book step-by-step, covering key aspects of both a tree-walk interpreter and a bytecode virtual machine.

## Key Concepts

### 1. **Tree-Walk Interpreter**

The interpreter is built by parsing the source code into an abstract syntax tree (AST) and then evaluating the tree.

>"Tree-walk interpreters have a simple and elegant implementation that makes them an excellent starting point for learning how interpreters work." — *Crafting Interpreters, Chapter 5*

### 2. **Scanner**

The scanner (or lexer) is responsible for converting the raw source code into a list of tokens that the parser can process.

>"A scanner, sometimes called a lexer, is the first phase in a compiler. It reads the source code as a sequence of characters and groups them into meaningful chunks called lexemes." — *Crafting Interpreters, Chapter 4*

### 3. **Parser**

The parser takes the list of tokens from the scanner and organizes them into a structure that reflects the grammar of the language.

>"The parser consumes the flat sequence of tokens produced by the scanner and organizes them into a hierarchical structure that reflects the nested grammatical structure of the source code." — *Crafting Interpreters, Chapter 6*

### 4. **AST (Abstract Syntax Tree)**

The AST represents the hierarchical structure of the source code. Each node in the tree corresponds to a construct in the language, such as an expression or statement.

>"An abstract syntax tree, or AST, is a tree representation of the abstract syntactic structure of source code written in a programming language." — *Crafting Interpreters, Chapter 7*

### 5. **Interpreter**

The interpreter walks the AST, visiting each node to evaluate expressions and execute statements.

>"Interpreters are at the heart of our understanding of programs and how they work. They give us a tool to reason about how a program will execute." — *Crafting Interpreters, Chapter 11*

### 6. **Error Handling**

Error handling is a critical part of building an interpreter. The book emphasizes the importance of providing clear, helpful error messages.

>"When your interpreter encounters an error, it should not simply halt execution with a cryptic message. Instead, it should try to provide meaningful feedback to help the user understand and correct the problem." — *Crafting Interpreters, Chapter 4*

### 7. **Bytecode Virtual Machine**

The project also explores creating a bytecode virtual machine for Lox, which involves compiling the AST into bytecode and then interpreting that bytecode.

>"A bytecode virtual machine is the heart of many modern interpreters. It’s a powerful, fast, and flexible way to implement an interpreter." — *Crafting Interpreters, Chapter 14*

## Getting Started

### Prerequisites

To build and run the Lox interpreter, you'll need the following:

- A C++ compiler that supports C++20 or higher.
- [CMake](https://cmake.org/) for generating build files.
- Basic understanding of programming concepts and C++.

### Building the Project

1. **Clone the repository:**

   ```sh
   git clone https://github.com/dutesier/lox.git
   cd lox-interpreter
   ```

2. **Generate build files with CMake:**

   ```sh
   mkdir build
   cd build
   cmake ..
   ```

3. **Build the project:**

   ```sh
   cmake --build .
   ```

### Running the Interpreter

After building the project, you can run the Lox interpreter from the command line.

1. **Run the interpreter: (TBD)**

   ```sh
   ./lox
   ```

2. **Execute Lox code:**

   You can run Lox code by typing commands directly into the interpreter or by passing a `.lox` file as an argument:

   ```sh
   ./lox script.lox
   ```

### Example Lox Program

```lox
print "Hello, World!";
```

### Current EBNF
```
expression     → literal
               | unary
               | binary
               | grouping ;

literal        → NUMBER | STRING | "true" | "false" | "nil" ;
grouping       → "(" expression ")" ;
unary          → ( "-" | "!" ) expression ;
binary         → expression operator expression ;
operator       → "==" | "!=" | "<" | "<=" | ">" | ">="
               | "+"  | "-"  | "*" | "/" ;
```

### License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.

### Acknowledgments

Special thanks to Robert Nystrom for writing *Crafting Interpreters* and providing the inspiration and guidance for this project.

---

Happy coding! 🌟