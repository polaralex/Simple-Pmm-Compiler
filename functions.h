extern int lex();

int error(char *string);
void addLexeme(char input_characters[30]);
char * getLexeme(int position);

int error(char *string){
	printf("Error: %s\n\n", string);
	printf(">> FAILURE: The input is not valid, according to the P-- Grammar <<\n\n");
	exit(EXIT_FAILURE);
	return(0);
}

// Global file pointer and Token variable:
static FILE *input;
static int token;
static int peekToken;

// Struct for Linked List for Lexemes (used in Lex and Syntax Analyzer):
typedef struct lexeme_list {
	char word[30];
	struct lexeme_list *next;
} lexeme;

static lexeme *lexeme_head;

void addLexeme(char input_characters[30]) {

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

// Global parsing output character arrays:
static char output[BIG_CHARACTER_ARRAY_LENGTH];
static char encodedOutput[BIG_CHARACTER_ARRAY_LENGTH];

// Global state of Lexicographical Analysis (checks if it's been already done once):
static int analysis_done = 0;

// Global variables for Intercode:
static int nextquadlabel = 100;