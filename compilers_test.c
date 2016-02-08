#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Changelog:
 1. In the current version, we have a working input scanner, that discerns
 between character input and whitespace. 
*/

int main(int argc, char *argv[]){

	printf("\nWelcome to the test program for the Compilers project.\n\n");

	size_t defaultSize = 50;

	FILE *fp;
	char stringContainer[255];
	char tempChar;
	int numberOfScannedCharacters = 0;
	int whitespaceCharacters = 0;

	// Cancel the program if there is no input argument:
	if(argc == 1){
		printf("There was no input file. Please give a text input to be processed as an argument\n\n");
		return (0);
	}

	// Open the file stream into the file pointer:
	fp = fopen(argv[1], "r");

	// Scan the characters and copy them into our buffer:
	while( (tempChar = getc(fp)) != EOF ){

		if( strncmp(&tempChar, " ", 1) != 0 ){
			strcat(stringContainer, &tempChar);
			numberOfScannedCharacters++;
		} else {
			whitespaceCharacters++;
		}
	}

	printf("File Scaning is Over:\nTotal Characters Read: %d.\nTotal Whitespace Ignored: %d\n\n",
		numberOfScannedCharacters, whitespaceCharacters);



}