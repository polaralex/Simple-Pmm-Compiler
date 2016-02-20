extern int lex(void);

void error(char *string);

void error(char *string){
	printf("Error: %s\n\n", string);
}

// Global file pointer and Token variable:
static FILE *fp;
static int token;

// Global parsing output character arrays:
static char lexOutput[BIG_CHARACTER_ARRAY_LENGTH];
static char encodedOutput[BIG_CHARACTER_ARRAY_LENGTH];

// Global state of Lexicographical Analysis (checks if it's been already done once):
static int analysis_done = 0;