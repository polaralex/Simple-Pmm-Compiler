extern int lex();

int error(char *string);

int error(char *string){
	printf("Error: %s\n\n", string);
	printf(">> FAILURE: The input is not valid, according to the P-- Grammar <<\n\n");
	exit(EXIT_FAILURE);
	return(0);
}

void getFilenameWithoutExtension(char nameoffile[30], char * filenameWithoutExtension);

// Global file pointer and Token variable:
static FILE *input;
static int token;
static int peekToken;
static char currentLexeme[30];
static int lexeme_parsing_counter = 0;
static int lexeme_counter = 0;

// Struct for Linked List for Lexemes (used in Lex and Syntax Analyzer):
typedef struct lexeme_list {
	char word[30];
	struct lexeme_list *next;
	int isItUsed;
} lexeme;

static lexeme *lexeme_head;

typedef struct temporary_variables_list {
	char variable[30];
	struct temporary_variables_list *next;
} temp_list;

static temp_list *temp_variables_head;

// Global parsing output character arrays:
static char output[BIG_CHARACTER_ARRAY_LENGTH];
static char encodedOutput[BIG_CHARACTER_ARRAY_LENGTH];

// Global state of Lexicographical Analysis (checks if it's been already done once):
static int analysis_done = 0;

// Global variables for Intercode:
static int nextquadlabel = 100;