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

# Usage:

1. Compile "compiler.c" using GCC (tested on OSX and Ubuntu Linux)
2. Open the "a.out" file using one of the provided test inputs (from the folder "/Inputs") as an argument.
3. If everything worked ok (the input code was valid, according to the language's grammar), three output files should have been produced inside the "/Inputs" folder.

Normally, the endcode .msim files work on a proprietary processor emulator called "Metasim", that is used in dept. of Computer Science, University of Ioannina. But, still, the final code is similar to many common endcode implementations.
