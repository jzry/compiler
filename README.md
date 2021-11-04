# Compiler

## About
A compiler translates higher level programming language instructions to equivalent (or near equivalent) assembly or machine code instructions that a CPU can use to execute. Higher level programming languages, such as C, C++, or Python, are great for development because they are easy to read, and so they cut the time required to develop a program as compared to writing a program in assembly or machine language, which is nearly illegible.
This C compiler contains four components: The virtual machine, the lexical analyzer, the parser (intermediate code generator), and the semantic analyzer. It is possible for compilers to contain more components for code optimization to make execution faster, however this specific implementation does not contain one.

## Input and Output
The main purpose of any computer is to process input given by a human or another machine to useful output that a human or another machine can use. This specific compiler receives input from a text file that has instructions written in a made up programming language. There are examples of these input files within the component directories to test out individual compiler components, such as the lexical analyzer. The output is displayed to the computer terminal (or shell, command line, etc).

## The Virtual Machine
The virtual machine component contains a memory space called the process address space (PAS) which is represented by a stack. The PAS contains memory registers and activation records which keep track of instructions throughout the program cycle.

## The Lexical Analyzer
The lexical analyzer component processes raw input (programming language code) and sorts out what each word (or lexeme) in the code represents. It tokenizes (or associates) each lexeme from the input to a hard coded value to create a token list of instructions that the CPU can use to carry out instructions.

## The Parser (Intermediate Code Generator)
The parser component takes the token list and determines whether a program is able to be produced if a code follows the language standard. In other words, it checks for language errors.

## The Semantic Analyzer
The semantic analyzer component takes the token list and analyzes the language to ensure standardization.
