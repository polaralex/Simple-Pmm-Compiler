extern int lex();

int error(char *string);

int error(char *string){
	printf("Error: %s\n\n", string);
	printf(">> FAILURE: The input is not valid, according to the P-- Grammar <<\n\n");
	exit(EXIT_FAILURE);
	return(0);
}

// Global file pointer and Token variable:
static FILE *fp;
static int token;
static int peekToken;

// Global parsing output character arrays:
static char lexOutput[BIG_CHARACTER_ARRAY_LENGTH];
static char encodedOutput[BIG_CHARACTER_ARRAY_LENGTH];

// Global state of Lexicographical Analysis (checks if it's been already done once):
static int analysis_done = 0;