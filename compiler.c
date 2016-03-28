#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "defines.h"
#include "functions.h"
#include "lex.c"
#include "syntax_analyzer.c"

int main(int argc, char *argv[]) {

	output[0] = '\0';
	encodedOutput[0] = '\0';

	printf("\f");
	printf("--------------------------------------------------------------\n");
	printf("--- Welcome to the test program for the Compilers project. ---\n");
	printf("--------------------------------------------------------------\n\n");

	// Cancel the program if there is no input argument:
	if(argc == 1){
		printf("<!> Error: There was no input file.\nPlease give a text input to be processed as an argument\n\n");
		return (0);
	}

	// Open the file stream into the file pointer:
	input = fopen(argv[1], "r");

	printf("--------------------------------------\n");
	printf("Status: Input File Opened for Parsing.\n");
	printf("--------------------------------------\n\n");

	// Start lex for the first time (it's going to make the analysis by itself) - and take the first token:
	token = lex();
	printf("Compiler: Token is %d \n\n", token);

	if ( analysis_done == 0 ) {
		error("Lexicographical Analysis could not be completed. Please, check the input for errors.\n");
	}

	// Then, continue to the Syntax Analyzer:
	program();

	// Finally, create (and populate) the Text file containing the Quads List:
	printQuadsToFile(quad_list_head);

	// And, also, create the C language equivalent:
	exportQuadsToCFile(quad_list_head);

}