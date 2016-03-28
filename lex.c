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
void checkNumberAndAddToOutput();
void checkCharacterForComplexAndAddIt();
void addLexeme(char input_characters[30]);
char * getLexeme(int position);

// Variables (global for helping with code-cleanup):
int nextToken;
size_t defaultSize = 50;
char stringContainer[BIG_CHARACTER_ARRAY_LENGTH];
char currentWord[BIG_CHARACTER_ARRAY_LENGTH];
char buffer[DEFAULT_WORD_ARRAY_SIZE];
int stateAnalyzerReturnStatus;
char tempCharArray[512];

char character1;
char character2;
int i = 0;
int closureState;
char tempForGetc;
int tempPunctuationIdentifier;
int state = STATE_WHITESPACE;

// The list of all the valid characters in our language:
const char validCharacters[74] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n',
							'o','p','q','r','s','t','u','v','w','x','y','z',
							'A','B','C','D','E','F','G','H','I','J','K','L','M','N',
							'O','P','Q','R','S','T','U','v','W','X','Y','Z',
							'0','1','2','3','4','5','6','7','8','9',
							'+','-','*','/','<','>','=',':',';',',','{','}'};

// If the parameter "actionRequest" is "2", "lex" will return the Peeked next Token:
int lex(){

	// If analysis isn't done yet, analyse. Else, return the next token:
	if (analysis_done == 0) {

		printf(" \n");
		printf("Starting lex. Analysis Done is: %d \n\n", analysis_done);

		stateAnalyzerReturnStatus = stateAnalyzer(input, output, encodedOutput);

		analysis_done = 1;

		printf("State analyzer return status = %d \n\n", stateAnalyzerReturnStatus);

		if(stateAnalyzerReturnStatus == STATE_EOF){

			// Return the final lexicographical translation:
			printf("-----------------------------\n");
			printf("Status: File Reading is Over.\n");
			printf("-----------------------------\n\n");
			printf("Result: The Lex Output is '%s' \n\n", output);
			printf("Result: The EncodedOutput is '%s' \n\n", encodedOutput);
			
			// Show the Lexeme List contents:
			printf("The Lexeme List contains: \n");
			int y;
			for( y=0; y<lexeme_counter; y++) {
				printf("%s, ", getLexeme(y));
			}
			printf("\n");

			// Here begins the tokenization of the encodedOutput string:
			nextToken = atol(strtok(encodedOutput, " "));

			// And here we fill the peekToken for the first time:
			char * tempTokenizerOutput;
			tempTokenizerOutput = strtok(NULL, " ");

			// And we fill the next Lexeme:
			char * temp_next_lexeme = getLexeme(lexeme_parsing_counter);
			strcpy(currentLexeme, temp_next_lexeme);
			lexeme_parsing_counter++;

			if ( tempTokenizerOutput != NULL ) {
				peekToken = atol(tempTokenizerOutput);
			} else {
				peekToken = STATE_EOF;
			}

			return ( nextToken );

		} else {

			printf("Error: Parsing could not be completed because of Errors.");

		}

	} else if ( analysis_done == 1 ) {

		// Note: When you pass the NULL-pointer as the first 'strtok' argument, it continues the parsing
		// from the place it was left at the first time. So, from now on, every time we call it, we will
		// get the next token of the 'encodedOutput' that is delimited by whitespace:

		// First, we move the peekToken to the nextToken to serve it to any function that requires it:
		nextToken = peekToken;

		if ( nextToken != STATE_EOF ) {

			// Then, we fill the now empty 'peekToken' with the up-next Token in the series:
			char * tempTokenizerOutput;
			tempTokenizerOutput = strtok(NULL, " ");
			
			if( tempTokenizerOutput != NULL ) {
				peekToken = atol(tempTokenizerOutput);
			} else {
				peekToken = STATE_EOF;
			}

			// Getting the next Lexeme in the list:
			char * temp_next_lexeme = getLexeme(lexeme_parsing_counter);
			strcpy(currentLexeme, temp_next_lexeme);
			lexeme_parsing_counter++;

			printf("Next token: %d \n", nextToken);
			printf("Peek token: %d \n", peekToken);
			printf("Next lexeme: %s \n\n", currentLexeme);

			return( nextToken );

		} else {

			printf("Debug: The TOKENIZER has reached the END of input!\n\n");
			peekToken = STATE_EOF;
			return( STATE_EOF );
		}
	}

	// The code (probably) never reaches this point:
	return(0);
}

int stateAnalyzer (FILE *input, char *output, char *encodedOutput) {

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
			
			initializeCharArray(currentWord);
			i=0;

			state = STATE_VARIABLE;

			if(i<30) {
				currentWord[i] = character1;
			};

			i++;

		} else if (state == STATE_VARIABLE && (isalpha(character1)||isdigit(character1)) ) { // State Variable: Getting letter or number after the first being a letter -> It's still a variable
			
			state = STATE_VARIABLE;

			printf("Inside building currentWord: '%s'\n", currentWord);

			if(i<30) {
				currentWord[i] = character1;
			};

			i++;

		} else if (state == STATE_VARIABLE && isspace(character1)) { // State Variable -> Whitespace: Ending a variable when encountering whitespace
			
			state = STATE_WHITESPACE;

			printf("Debug Sensitive Word Identification: Current word contains: %s \n", currentWord);

			// Here we add the variable identifier OR the sensitive identifier to our text buffers (encoded and original).
			int isItSensitiveWord = isSensitiveWord(currentWord);

			// Original:
			strncat(output, currentWord, i);
			strncat(output, " ", i);
			addLexeme(currentWord);

			// Encoded:
			if( isItSensitiveWord == NOT_SENSITIVE ){
				sprintf(encodedOutput + strlen(encodedOutput), "%d ", VARIABLE);
			} else {
				sprintf(encodedOutput + strlen(encodedOutput), "%d ", isItSensitiveWord);
			}

			initializeCharArray(currentWord);
			i=0; 

		} else if (state == STATE_VARIABLE && ispunct(character1)) {

			printf("Inside currentWord: '%s'\n", currentWord);

			state = STATE_WHITESPACE;

			// Here we add the variable identifier OR the sensitive identifier to our text buffers (encoded and original).
			int isItSensitiveWord = isSensitiveWord(currentWord);

			// Original:
			strncat(output, currentWord, i);
			addLexeme(currentWord);
			strncat(output, " ", i);

			// Encoded:
			if( isItSensitiveWord == NOT_SENSITIVE ){
				sprintf(encodedOutput + strlen(encodedOutput), "%d ", VARIABLE);
			} else {
				sprintf(encodedOutput + strlen(encodedOutput), "%d ", isItSensitiveWord);
			}

			initializeCharArray(currentWord);
			i=0;

			checkCharacterForComplexAndAddIt();


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
			
			checkNumberAndAddToOutput();

		} else if (state == STATE_NUMBER && ispunct(character1)) {

			checkNumberAndAddToOutput();

			// ...and then, we check the punctuation symbol that showed up (if it's simple or complex character):

			state = STATE_WHITESPACE;

			initializeCharArray(currentWord);
			i=0;

			checkCharacterForComplexAndAddIt();

		// Here begin the composite punctuation identifications:
		} else if (state == STATE_WHITESPACE && ispunct(character1)) {

			state = STATE_WHITESPACE;

			initializeCharArray(currentWord);
			i=0;

			checkCharacterForComplexAndAddIt();

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

	if( character == '+' | character == '-' | character == '*' | character == '=' | character == ',' | character == ';' | character == '{'| character == '[' | character == '(' | character == '}' | character == ']' | character == ')') {
		return (SIMPLE_CHARACTER);
	} else if ( character == '<'| character == '>'| character == ':' | character == '/') {
		return (POSSIBLE_COMPLEX_CHAR);
	} else {
		return (STATE_ERROR);
	}

}

void initializeCharArray (char *characterArray) {

	int i=0;

	for(i=0; i<BIG_CHARACTER_ARRAY_LENGTH; i++){
		characterArray[i] = '\0';
	};
}

char peek(FILE *stream) {

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
	} else {
		return(NOT_SENSITIVE);
	}
}

void checkNumberAndAddToOutput() {
	// First, we save the number that we have until now (if it's valid):
	char* ptr;

	// Check for the integer size restriction (between -32767 and 32767):
	if ( abs((int)strtol(currentWord, &ptr, 10)) < 32767 ) {

		state = STATE_WHITESPACE;

		sprintf(output + strlen(output), "%s ", currentWord);
		addLexeme(currentWord);
		sprintf(encodedOutput + strlen(encodedOutput), "%d ", INTEGER);

		initializeCharArray(currentWord);

		i=0;

	} else {

			error("[Lexicographical Analyzer] Integers must be between -32767 and 32767.");
			state = STATE_ERROR;
	}
}

void checkCharacterForComplexAndAddIt() {

	closureState = doesCharRequireClosure(character1);

	printf("Does char require closure? Answer: %d\n\n", closureState);

	// Discern between a common punctuation character and one that needs closure:
	if( closureState == SIMPLE_CHARACTER ){

		strncat(output, &character1, 1);
		strncat(output, " ", 1);
		addLexeme("ENCODED");

		tempPunctuationIdentifier = getPunctuationIdentifier(character1);

		printf("The punctuation identifier is: %d\n\n", tempPunctuationIdentifier);

		sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

	} else if ( closureState == POSSIBLE_COMPLEX_CHAR ) {

			if ( character1 == '<' && character2 != '=' && character2 != '>') {

				strncat(output, "<", 1);
				strncat(output, " ", 1);
				addLexeme("ENCODED");

				tempPunctuationIdentifier = getPunctuationIdentifier('<');
				sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

			} else if( character1 == '<' && character2 == '=') {

				getc(input); // Get the peeked character out of the way for the next loop
				strncat(output, "<=", 2);
				strncat(output, " ", 1);
				addLexeme("ENCODED");

				tempPunctuationIdentifier = lessequals;
				sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

			} else if ( character1 == '<' && character2 == '>') {

				getc(input); // Get the peeked character out of the way for the next loop
				strncat(output, "<>", 2);
				strncat(output, " ", 1);
				addLexeme("ENCODED");

				tempPunctuationIdentifier = different;
				sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

			} else if ( character1 == '>' && character2 == '=') {

				getc(input); // Get the peeked character out of the way for the next loop
				strncat(output, ">=", 2);
				strncat(output, " ", 1);
				addLexeme("ENCODED");

				tempPunctuationIdentifier = moreequals;
				sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

			} else if ( character1 == '>' && character2 != '=') {

				strncat(output, ">", 1);
				strncat(output, " ", 1);
				addLexeme("ENCODED");

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

			} else if ( character1 == '/' && character2 != '*') {

				strncat(output, "/", 1);
				strncat(output, " ", 1);
				addLexeme("ENCODED");

				tempPunctuationIdentifier = getPunctuationIdentifier('/');
				sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

			} else if ( character1 == ':' && character2 == '=' ) {

				getc(input);
				strncat(output, ":=", 2);
				strncat(output, " ", 1);
				addLexeme("ENCODED");

				tempPunctuationIdentifier = assign;
				sprintf(encodedOutput + strlen(encodedOutput), "%d ", tempPunctuationIdentifier);

			} else if ( character1 == '*' && character2 == '/'){

				// This error happens if we find a comment-close character before we find a comment-open one:
				state = STATE_ERROR;
			}

	} else if (closureState == STATE_ERROR){

		state = STATE_ERROR;

	}
}

void addLexeme(char input_characters[30]) {

	lexeme_counter++;

	// Create the new node to be inserted in our list:
	lexeme *new_lexeme = malloc(sizeof(lexeme));

	// Add the data to the new lexeme struct:
	strcpy(new_lexeme->word, input_characters);
	new_lexeme->next = NULL;

	// If the list is empty, make this new node the head:
	if ( lexeme_head == NULL ) {

		lexeme_head = new_lexeme;

	} else {

		lexeme *current = lexeme_head;

		while ( current->next != NULL ) {
			current = current->next;
		}

		// We reach this point, when we are at the last node:
		current->next = new_lexeme;
	}
}

char * getLexeme(int position) {

	int i=0;

	lexeme *lexemePointer = lexeme_head;

	while ( lexemePointer != NULL && i <= position ) {

		if (i == position ) {
			return (lexemePointer->word);
		}

		lexemePointer = lexemePointer->next;
		i++;
	}

	return(" ");
}