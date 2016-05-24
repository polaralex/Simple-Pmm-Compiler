# Simple-Pmm-Compiler
A simple compiler for a test language called P--, made for educational purposes.

It's a project in C, that implements the basic functionality of a standard Programming Language Compiler.

This project was developed for the Compilers class at the Computer Science Department, University of Ioannina.

# Description of the Source Files:

1. compiler.c: The file that includes the main() and calls the other functions.
2. syntax_analyzer.c: Does the Syntax Analyzing to check if the input is grammatically correct (according to the provided language's specifications)
3. intercode_functions.c: Contains required function for the intercode generation
4. symbol_table.c: Holds the data about the Scopes and Names of Functions and Variables.
5. endcode.c: Contains that functions required for the production of .msim files, containing the Endcode.
