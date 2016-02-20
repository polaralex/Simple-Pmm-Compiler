#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "defines.h"

int isCharacterValid(char character);
int stateAnalyzer (FILE *input, char *output, char *encodedOutput);
int doesCharRequireClosure (char character);
void initializeCharArray (char *characterArray);
int isSensitiveWord (char *inputString);
int getPunctuationIdentifier(char inputCharacter);
char peek(FILE *stream);

// The list of all the valid characters in our language:
const char validCharacters[74] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n',
							'o','p','q','r','s','t','u','v','w','x','y','z',
							'A','B','C','D','E','F','G','H','I','J','K','L','M','N',
							'O','P','Q','R','S','T','U','v','W','X','Y','Z',
							'0','1','2','3','4','5','6','7','8','9',
							'+','-','*','/','<','>','=',':',';',',','{','}'};

int lex(){

	// Temporary variable:
	int nextToken;

	// Variables and Constants:
	size_t defaultSize = 50;

	char stringContainer[BIG_CHARACTER_ARRAY_LENGTH];
	char currentWord[BIG_CHARACTER_ARRAY_LENGTH];

	char buffer[DEFAULT_WORD_ARRAY_SIZE];

	int stateAnalyzerReturnStatus;

	// Used for strtol():
	char tempCharArray[256];

	printf("Starting lex. Analysis Done is: %d \n\n", analysis_done);

	// If analysis isn't done yet, analyse. Else, return the next token:
	if (analysis_done == 0) {

		stateAnalyzerReturnStatus = stateAnalyzer(fp, lexOutput, encodedOutput);

		analysis_done = 1;

		printf("State analyzer return status = %d \n\n", stateAnalyzerReturnStatus);

		if(stateAnalyzerReturnStatus == STATE_EOF){

			// Return the final lexicographical translation:
			printf("-----------------------------\n");
			printf("Status: File Reading is Over.\n");
			printf("-----------------------------\n\n");
			printf("Result: The LexOutput is '%s' \n\n", lexOutput);
			printf("Result: The EncodedOutput is '%s' \n\n", encodedOutput);

			// Here begins the tokenization of th encodedOutput string:
			nextToken = atol(strtok(encodedOutput, " "));

			return ( nextToken );

		} else {

			printf("Error: Parsing could not be completed because of Errors.");

		}

	} else {

		// Note: When you pass the NULL-pointer as the first 'strtok' argument, it continues the parsing
		// from the place it was left at the first time. So, from now on, every time we call it, we will
		// get the next token of the 'encodedOutput' that is delimited by whitespace:

		char * tempTokenizerOutput;
		tempTokenizerOutput = strtok(NULL, " ");

		if ( tempTokenizerOutput != NULL ) {

			nextToken = atol(tempTokenizerOutput);
			printf("Next token: %d \n", nextToken);
			return( nextToken );

		} else {

			printf("Debug: The TOKENIZER has reached the END of input!\n\n");
			return( STATE_EOF );

		}

	}

	// The code (probably) never reaches this point:
	return(0);

}

int stateAnalyzer (FILE *input, char *output, char *encodedOutput) {

	char currentWord[DEFAULT_WORD_ARRAY_SIZE];

	char character1;
	char character2;
	int i = 0;

	int closureState;
	char tempForGetc;

	int tempPunctuationIdentifier;

	int state = STATE_WHITESPACE;

	initializeCharArray(currentWord);

	while ( state != STATE_ERROR && state != STATE_EOF ){

		// Reading the next and peeking at the second next character:
		character1 = getc(input);
		character2 = peek(input);

		// Debug:
		printf("State: %d \n", state);
		printf("Character 1 = '%c' \n", character1);
		printf("Character 2 = '%c' \n\n", character2);

		if ( character1 == EOF ) {

			state = STATE_EOF;

		} else if (state == STATE_WHITESPACE && isspace(character1)) { // State Whitespace: Loop getting whitespace.

			state = STATE_WHITESPACE;

		} else if (state == STATE_WHITESPACE && isalpha(character1)) { // State Variable: Starts with alphanumeric -> It's a variable
			
			state = STATE_VARIABLE;

			if(i<30) {
				currentWord[i] = character1;
			};

			i++;

		} else if (state == STATE_VARIABLE && (isalpha(character1)||isdigit(character1)) ) { // State Variable: Getting letter or number after the first being a letter -> It's still a variable
			
			state = STATE_VARIABLE;

			if(i<30) {
				currentWord[i] = character1;
			};

			i++;

		} else if (state == STATE_VARIABLE && isspace(character1)) { // State Variable -> Whitespace: Ending a variable when encountering whitespace
			
			state = STATE_WHITESPACE;

			// Here we add the variable identifier OR the sensitive identifier to our text buffers (encoded and original).
			int isItSensitiveWord = isSensitiveWord(currentWord);

			// Original:
			strncat(output, currentWord, i);
			strncat(output, " ", i);

			// Encoded:
			if( isItSensitiveWord == NOT_SENSITIVE ){
				sprintf(encodedOutput + strlen(encodedOutput), "%d ", VARIABLE);
			} else {
				sprintf(encodedOutput + strlen(encodedOutput), "%d ", isItSensitiveWord);
			}

			initializeCharArray(currentWord);
			i=0; 

		} else if (state == STATE_VARIABLE && ispunct(character1)) {

			// NOTE: This part of code is DUPLICATED below.
			// Maybe I will clean this up later and make it
			// a discrete function.

			state = STATE_WHITESPACE;

			// Here we add the variable identifier OR the sensitive identifier to our text buffers (encoded and original).
			int isItSensitiveWord = isSensitiveWord(currentWord);

			// Original:
			strncat(output, currentWord, i);
			strncat(output, " ", i);

			// Encoded:
			if( isItSensitiveWord == NOT_SENSITIVE ){
				sprintf(encodedOutput + strlen(encodedOutput), "%d ", VARIABLE);
			} else {
				sprintf(encodedOutput + strlen(encodedOutput), "%d ", isItSensitiveWord);
			}

			initializeCharArray(currentWord);
			i=0;

			closureState = doesCharRequireClosure(character1);

			printf("Does char require closure? Answer: %d\n\n", closureState);

			// Discern between a common punctuation character and one that needs closure:
			if( closureState == SIMPLE_CHARACTER ){

				strncat(output, &character1, 1);
				strncat(output, " ", 1);

				tempPunctuationIdentifier = getPunctuationIdentifier(character1);

				printf("The punctuation identifier is: %d\n\n", tempPunctuationIdentifier);

				sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

			} else if ( closureState == POSSIBLE_COMPLEX_CHAR ) {

					if ( character1 == '<' && character2 != '=' && character2 != '>') {

						strncat(output, "<", 1);
						strncat(output, " ", 1);

						tempPunctuationIdentifier = getPunctuationIdentifier('<');
						sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

					} else if( character1 == '<' && character2 == '=') {

						getc(input); // Get the peeked character out of the way for the next loop
						strncat(output, "<=", 2);
						strncat(output, " ", 1);

						tempPunctuationIdentifier = lessequals;
						sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

					} else if ( character1 == '<' && character2 == '>') {

						getc(input); // Get the peeked character out of the way for the next loop
						strncat(output, "<>", 2);
						strncat(output, " ", 1);

						tempPunctuationIdentifier = different;
						sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

					} else if ( character1 == '>' && character2 == '=') {

						getc(input); // Get the peeked character out of the way for the next loop
						strncat(output, ">=", 2);
						strncat(output, " ", 1);

						tempPunctuationIdentifier = moreequals;
						sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

					} else if ( character1 == '>' && character2 != '=') {

						strncat(output, ">", 1);
						strncat(output, " ", 1);

						tempPunctuationIdentifier = getPunctuationIdentifier('>');
						sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

					}  else if ( character1 == '/' && character2 == '*') {

						printf("We know that a comment section begins.\n\n");

						// Here we know that comments open, so we go through the input to find
						// their closing point, and ignore everything between the comment symbols:

						character1 = getc(input);
						character2 = peek(input);

						printf("Character1: %c\n", character1);
						printf("Character2: %c\n\n", character2);

						while ( (character1 != '*') || (character2 != '/') ) {

							character1 = getc(input);
							character2 = peek(input);

							printf("loop.\n");

							if( character1 == EOF ){
								// This means that the file ends before the comments close
								state = STATE_ERROR;
							}
						}

						getc(input);
						 // Get the peeked character (in our case '/') out of the way

					} else if ( character1 == '*' && character2 == '/'){

						// This error happens if we find a comment-close character before we find a comment-open one:
						state = STATE_ERROR;
					}


			} else if (closureState == REQUIRES_CLOSURE || closureState == IS_CLOSURE_CHARACTER) {

				// To-Do: Maybe we will have to check for closure - but I don't know yet
				
				strncat(output, &character1, 1);
				strncat(output, " ", 1);

				tempPunctuationIdentifier = getPunctuationIdentifier(character1);
				sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

			} else if (closureState == STATE_ERROR){

				state = STATE_ERROR;

			}


		} else if (state == STATE_WHITESPACE && isdigit(character1)) { // State Number: Starts with digit -> It's an integer
			
			state = STATE_NUMBER;

			if(i<30) {
				strncat(currentWord, &character1, 1);
			};

			i++;

		} else if (state == STATE_NUMBER && isdigit(character1)) { // State Number: And we keep getting numbers
			
			state = STATE_NUMBER;

			if(i<30) {
				strncat(currentWord, &character1, 1);
			};

			i++;

		} else if (state == STATE_NUMBER && isalpha(character1)) { // We are in state Number and we get a letter, so, ERROR.

			state = STATE_ERROR;

		} else if (state == STATE_NUMBER && isspace(character1)) { // The number has ended with whitespace, so we save it:
			
			state = STATE_WHITESPACE;

			sprintf(output + strlen(output), "%s ", currentWord);
			sprintf(encodedOutput + strlen(encodedOutput), "%d ", INTEGER);

			initializeCharArray(currentWord);
			i=0;

		// Here begin the composite punctuation identifications:
		} else if (state == STATE_WHITESPACE && ispunct(character1)) {

			closureState = doesCharRequireClosure(character1);

			printf("Does char require closure? Answer: %d\n\n", closureState);

			// Discern between a common punctuation character and one that needs closure:
			if( closureState == SIMPLE_CHARACTER ){

				strncat(output, &character1, 1);
				strncat(output, " ", 1);

				tempPunctuationIdentifier = getPunctuationIdentifier(character1);

				printf("The punctuation identifier is: %d\n\n", tempPunctuationIdentifier);

				sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

			} else if ( closureState == POSSIBLE_COMPLEX_CHAR ) {

					if ( character1 == '<' && character2 != '=' && character2 != '>') {

						strncat(output, "<", 1);
						strncat(output, " ", 1);

						tempPunctuationIdentifier = getPunctuationIdentifier('<');
						sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

					} else if( character1 == '<' && character2 == '=') {

						getc(input); // Get the peeked character out of the way for the next loop
						strncat(output, "<=", 2);
						strncat(output, " ", 1);

						tempPunctuationIdentifier = lessequals;
						sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

					} else if ( character1 == '<' && character2 == '>') {

						getc(input); // Get the peeked character out of the way for the next loop
						strncat(output, "<>", 2);
						strncat(output, " ", 1);

						tempPunctuationIdentifier = different;
						sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

					} else if ( character1 == '>' && character2 == '=') {

						getc(input); // Get the peeked character out of the way for the next loop
						strncat(output, ">=", 2);
						strncat(output, " ", 1);

						tempPunctuationIdentifier = moreequals;
						sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

					} else if ( character1 == '>' && character2 != '=') {

						strncat(output, ">", 1);
						strncat(output, " ", 1);

						tempPunctuationIdentifier = getPunctuationIdentifier('>');
						sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

					}  else if ( character1 == '/' && character2 == '*') {

						printf("We know that a comment section begins.\n\n");

						// Here we know that comments open, so we go through the input to find
						// their closing point, and ignore everything between the comment symbols:

						character1 = getc(input);
						character2 = peek(input);

						printf("Character1: %c\n", character1);
						printf("Character2: %c\n\n", character2);

						while ( (character1 != '*') || (character2 != '/') ) {

							character1 = getc(input);
							character2 = peek(input);

							printf("loop.\n");

							if( character1 == EOF ){
								// This means that the file ends before the comments close
								state = STATE_ERROR;
							}
						}

						getc(input);
						 // Get the peeked character (in our case '/') out of the way

					} else if ( character1 == '*' && character2 == '/'){

						// This error happens if we find a comment-close character before we find a comment-open one:
						state = STATE_ERROR;
					}


			} else if (closureState == REQUIRES_CLOSURE || closureState == IS_CLOSURE_CHARACTER) {

				// To-Do: Maybe we will have to check for closure - but I don't know yet
				
				strncat(output, &character1, 1);
				strncat(output, " ", 1);

				tempPunctuationIdentifier = getPunctuationIdentifier(character1);
				sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

			} else if (closureState == STATE_ERROR){

				state = STATE_ERROR;

			}

		}
	}

	if (state == STATE_EOF) {

		printf("Parsing ended with STATE_EOF.\n\n");

		printf("This is inside output: %s \n\n", output);
		printf("This is inside encodedOutput: %s \n\n", encodedOutput);

		return(STATE_EOF);

	} else {

		return(STATE_ERROR);

	}

}

int isCharacterValid (char character) {

	int i=0;

	// First we check if the current character belongs to our valid alphabet:
	for(i=0; i<74; i++){
		if( character == validCharacters[i] ){
			return(1);
		}
	}
	// If no valid equivalent found:
	return(0);
}

int doesCharRequireClosure (char character) {

	printf("The character to be examined is %c\n\n", character);

	if( character == '+' | character == '-' | character == '=' | character == ',' | character == ';') {
		return (SIMPLE_CHARACTER);
	} else if ( character == '<'| character == '>'| character == ':' | character == '/') {
		return (POSSIBLE_COMPLEX_CHAR);
	} else if ( character == '{'| character == '[' | character == '(') {
		return (REQUIRES_CLOSURE);
	} else if ( character == '}' | character == ']' | character == ')') {
		return (IS_CLOSURE_CHARACTER);
	} else {
		return (STATE_ERROR);
	}

}

void initializeCharArray (char *characterArray){

	int i=0;

	for(i=0; i<(sizeof(characterArray)/sizeof(characterArray[0])); i++){
		characterArray[i] = '\0';
	};
}

char peek(FILE *stream)
{
    char c;
    c = fgetc(stream);
    ungetc(c, stream);

    return c;
}

int isSensitiveWord (char *inputString){

	// Sensitive words
	if( strcmp(inputString, "and") == 0){
		return (and_a);
	} else if( strcmp(inputString, "var") == 0){
		return (var_a);
	} else if( strcmp(inputString, "do") == 0){
		return (do_a);
	} else if( strcmp(inputString, "else") == 0){
		return (else_a);
	} else if( strcmp(inputString, "procedure") == 0){
		return (procedure_a);
	} else if( strcmp(inputString, "function") == 0){
		return (function_a);
	} else if( strcmp(inputString, "print") == 0){
		return (print_a);
	} else if( strcmp(inputString, "if") == 0){
		return (if_a);
	} else if( strcmp(inputString, "in") == 0){
		return (in_a);
	} else if( strcmp(inputString, "inout") == 0){
		return (inout_a);
	} else if( strcmp(inputString, "not") == 0){
		return (not_a);
	} else if( strcmp(inputString, "for") == 0){
		return (for_a);
	} else if( strcmp(inputString, "to") == 0){
		return (to_a);
	} else if( strcmp(inputString, "step") == 0){
		return (step_a);
	} else if( strcmp(inputString, "program") == 0){
		return (program_a);
	} else if( strcmp(inputString, "or") == 0){
		return (or_a);
	} else if( strcmp(inputString, "return") == 0){
		return (return_a);
	} else if( strcmp(inputString, "while") == 0){
		return (while_a);
	} else if( strcmp(inputString, "call") == 0){
		return (call_a);
	} else {
		return(NOT_SENSITIVE);
	}
}

int getPunctuationIdentifier(char inputCharacter){

	if( inputCharacter == '+' ){
		return (plus);
	} else if( inputCharacter == '-' ) {
		return (minus);
	} else if( inputCharacter == '*' ) {
		return (multipl);
	} else if( inputCharacter == '/' ) {
		return (divide);
	} else if( inputCharacter == '<' ) {
		return (lessthan);
	} else if( inputCharacter == '>' ) {
		return (morethan);
	} else if( inputCharacter == '=' ) {
		return (equals);
	/*} else if( strcmp(inputString, "<=") == 0){
		return (lessequals);
	} else if( strcmp(inputString, ">=") == 0){
		return (moreequals);
	} else if( strcmp(inputString, "<>") == 0){
		return (different);
	} else if( strcmp(inputString, ":=") == 0){*/
		return (assign);
	} else if( inputCharacter == ';' ) {
		return (semicolon);
	} else if( inputCharacter == ',' ) {
		return (comma);
	} else if( inputCharacter == '{' ) {
		return (curlbrackleft);
	} else if( inputCharacter == '}' ) {
		return (curlbrackright);
	} else if( inputCharacter == '(' ) {
		return (parenthleft);
	} else if( inputCharacter == ')' ) {
		return (parenthright);
	} else if( inputCharacter == '[' ) {
		return (bracketleft);
	} else if( inputCharacter == ']' ) {
		return (bracketright);
	/*} else if( strcmp(inputString, "/ *") == 0){
		return (commentbegin);
	} else if( strcmp(inputString, "* /") == 0){
		return (commentend);*/
	} else {
		return(NOT_SENSITIVE);
	}
}