
//FAILSAFE FOR FILE OUTPUT LENGTH:
int failsafe = 0;

// Struct for the general representation of Quads:
typedef struct quad {
	int label;
	char operator[30];
	char argument1[30];
	char argument2[30];
	char result[30];
} quartet;

// The list that stores the produced Quads for the intercode:
typedef struct quartet_list {
	struct quad quartet;
	struct quartet_list *next;
} quartet_node;

// List that holds only label numbers:
typedef struct label_list {
	int label;
	struct label_list *next;
} label_node;

// Helper Subroutines:
int nextquad();
void genquad(char operator[30], char argument1[30], char argument2[30], char result[30]);
char * newtemp();
void addToTempVariablesList(char *variable);

// List pointers:
quartet_node *quad_list_head; //This is the genquad list.

quartet_node *previousCurrentEndcode;
quartet_node *currentEndcode;
int checkForEndcodeQuadPointerIfAlreadyInitialized = 0;

quartet_node *quad_list_endcode_pointer;
label_node *label_head;

label_node * emptylist();
label_node * makelist(int quad_label);
label_node * merge(struct label_list * list1, struct label_list * list2);
void backpatch(struct label_list * list, int label_number);

int temp_number = 1;

int nextquad() {
	return(nextquadlabel);
}

void genquad (char operator[30], char argument1[30], char argument2[30], char result[30]) {

	// Generate a Quad and add it in the Quad list:
	quartet_node *current = quad_list_head;

	// Create the new node to be inserted in our list:
	quartet_node *new_node = malloc(sizeof(quartet_node));

	new_node->quartet.label = nextquadlabel;
	nextquadlabel = nextquadlabel + 10;

	strcpy(new_node->quartet.operator, operator);
	strcpy(new_node->quartet.argument1, argument1);
	strcpy(new_node->quartet.argument2, argument2);
	strcpy(new_node->quartet.result, result);

	new_node->next = NULL;

	// If the list is empty, make this new node the head:
	if ( quad_list_head == NULL ) {

		quad_list_head = new_node;

	} else {

		while( current->next != NULL ) {

			current = current->next;

		}

		// We reach this point, when we are at the last node:
		current->next = new_node;
		current->next->next = NULL;
	}
}

// Creates and Returns a Temporary variable (in the format of "T_1", "T_2" etc.):
char * newtemp() {

	char *temp = (char *) malloc (sizeof(30));
	sprintf( temp, "T_%d", temp_number);

	temp_number = temp_number + 1;

	// This is used for the C-equivalent
	// intermediate code production:
	printf("In here!!!\n\n");
	char temp_copy[30];
	sprintf( temp_copy, "T_%d", temp_number);
	addToTempVariablesList(temp_copy);

	return temp;
}

// Creates and Returns an Empty List of Quads:
label_node * emptylist() {

	label_node *temp_list = NULL;
	return temp_list;
}

// Creates and Returns a List of Quad-Labels with one Label element:
label_node * makelist(int quad_label) {

	label_node *temp_list = malloc(sizeof(label_node));
	temp_list -> label = quad_label;
	temp_list -> next = NULL;

	return temp_list;
}

// Merges two Lists of Quad-Labels and Returns the Sorted List Result:
label_node * merge (struct label_list *list1, struct label_list
 * list2) {

	// Posibility of one of the lists being NULL:
	if ( list1 == NULL ) {
		return(list2);
	}

	if ( list2 == NULL ) {
		return(list1);
	}

	label_node * temp_list;

	// Swap the lists if list2's label is smaller that list1's.
	// In the end, we will hold the smallest value in list1:
	if ( (list1 -> label) > (list2 -> label) ) {
		temp_list = list2;
		list2 = list1;
		list1 = temp_list;
	}

	label_node *tail = list1;

	do {
		// Sort the list while looping, by finding any smaller label
		// than the first node of list2, and adding them to our merged list:
		while ( tail->next && (tail->next->label <= list2->label )) {
			tail = tail->next;
		}

		temp_list = tail->next;
		tail->next = list2;
		tail = list2;
		list2 = temp_list;

	} while (list2);

	return (list1);
}

void backpatch(struct label_list *list, int label_number) {

	// Set current to the head of the quartets list:
	quartet_node *current = quad_list_head;

	// Set label_list as the head of the list of labels:
	label_node *temp = list;

	while (current != NULL && temp != NULL) {

		if ( temp->label == current->quartet.label ) {
			sprintf (current->quartet.result, "%d", label_number);
			temp = temp->next;
		}

		current = current->next;
	}
}

void addToTempVariablesList(char *variable) {

	// Generate a Quad and add it in the Quad list:
	temp_list *current_temp = temp_variables_head;

	// Create the new node to be inserted in our list:
	temp_list *new_node = malloc(sizeof(temp_list));
	strcpy(new_node->variable, variable);
	new_node->next = NULL;

	// If the list is empty, make this new node the head:
	if ( temp_variables_head == NULL ) {

		temp_variables_head = new_node;

	} else {

		while( current_temp->next != NULL ) {

			current_temp = current_temp->next;
		}

		// We reach this point, when we are at the last node:
		current_temp->next = new_node;
		current_temp->next->next = NULL;
	}
}

void printQuadsToFile(char nameoffile[30], quartet_node *quadsList) {

	printf("-- Starting Export of Quads to .int file --\n\n");

	char filename[30];
	strcpy(filename, nameoffile);
	strcat(filename, ".int");
	FILE *quadsOutputFile = fopen(filename, "w");

	if (quadsOutputFile == NULL) {
    	printf("Error opening file!\n");
    	exit(1);
	}

	fprintf(quadsOutputFile, "\n");

	while (quadsList != NULL && failsafe < 100) {

		// Temporary holders for the output String values:
		char label[30];
		char op[30];
		char arg1[30];
		char arg2[30];
		char result[30];

		// Fill the holders using data from the quads struct:
		sprintf(label, "%d", quadsList->quartet.label);
		strcpy(op, quadsList->quartet.operator);
		strcpy(arg1, quadsList->quartet.argument1);
		strcpy(arg2, quadsList->quartet.argument2);
		strcpy(result, quadsList->quartet.result);

		// Write the holders' data to the text file:
		fprintf(quadsOutputFile, "%s: ", label);
		fprintf(quadsOutputFile, "%s , ", op);
		fprintf(quadsOutputFile, "%s , ", arg1);
		fprintf(quadsOutputFile, "%s , ", arg2);
		fprintf(quadsOutputFile, "%s\n", result);

		quadsList = quadsList->next;

		// THIS IS A FAILSAFE MECHANISM TO PREVENT NEVERENDING LOOP OUTPUTS:
		failsafe++;
	}

	fprintf(quadsOutputFile, "\n");
	fclose(quadsOutputFile);
}

void exportQuadsToCFile(char nameoffile[30], quartet_node *quadsList) {

	printf("-- Starting Export of Quads to C-equivalent file --\n\n");

	char filename[30];
	strcpy(filename, nameoffile);
	strcat(filename, ".c");
	FILE *cOutputFile = fopen(filename, "w");

	if (cOutputFile == NULL) {
    	printf("Error opening file!\n");
    	exit(1);
	}

	// First, print the basic structure of the C file:
	fprintf(cOutputFile, "\nint main() {\n");

	// Then, we have to add all the lexemes used
	//as integer variables:
	fprintf(cOutputFile, "\nint ");

		// Here, we add all the used lexemes as variables:
		int i=0;

		lexeme *lexemes = lexeme_head;

		// This is used to decide if a comma should be added
		// after each variable fprintf:
		int commaFlag = 0;

		while ( lexemes != NULL ) {

			if ( lexemes->isItUsed == 1 ) {

				if( commaFlag == 1 ) {
					fprintf(cOutputFile, ", ");
				} else {
					fprintf(cOutputFile, " ");
				}

				fprintf(cOutputFile, "%s", lexemes->word);

				commaFlag = 1;
			}

			lexemes = lexemes->next;
			i++;
		}

	fprintf(cOutputFile, ";\n");

	// And here we add the 'T_#' type variables that have been created
	// during intermediate code production:
	fprintf(cOutputFile, "int ");

		// Here, we add all the used lexemes as variables:
		i=0;

		temp_list *current_tmp = temp_variables_head;

		while ( current_tmp != NULL ) {

				fprintf(cOutputFile, "%s", current_tmp->variable);

				if( current_tmp->next == NULL ) {
					fprintf(cOutputFile, "");
				} else {
					fprintf(cOutputFile, ", ");
				}

		current_tmp = current_tmp->next;
	}

	fprintf(cOutputFile, ";\n\n");

	while (quadsList != NULL && failsafe < 1000) {

		// Temporary holders for the output String values:
		int labelNumber;
		char label[30];
		char op[30];
		char arg1[30];
		char arg2[30];
		char result[30];

		// Fill the holders using data from the quads struct:
		labelNumber = quadsList->quartet.label;
		sprintf(label, "%d", quadsList->quartet.label);
		strcpy(op, quadsList->quartet.operator);
		strcpy(arg1, quadsList->quartet.argument1);
		strcpy(arg2, quadsList->quartet.argument2);
		strcpy(result, quadsList->quartet.result);

		// 1. Add the label:
		fprintf(cOutputFile, "L_%d: ", labelNumber);

		// 2. 'Translate' the quad to C format.

		// 2.1 -> Basic Operations:
		if(strcmp(op,"+") == 0 || strcmp(op,"-") == 0 || strcmp(op,"*") == 0 || strcmp(op,"/") == 0) {

			fprintf(cOutputFile, "%s = %s %s %s; ", result, arg1, op, arg2);
		}

		// 2.2 -> Assignment:
		if(strcmp(op,":=") == 0 ) {

			fprintf(cOutputFile, "%s = %s; ", result, arg1);
		}

		// 2.3 -> Relational Operation (except equals):
		if(strcmp(op,"<") == 0 || strcmp(op,">") == 0 || strcmp(op,"<=") == 0 || strcmp(op,">=") == 0) {

			fprintf(cOutputFile, "if (%s %s %s) goto %s; ", arg1, op, arg2, result);
		}

		// 2.3 -> ...equals:
		if( strcmp(op,"=") == 0 ) {

			fprintf(cOutputFile, "if (%s == %s) goto %s; ", arg1, arg2, result);
		}

		// 2.4 -> Jump:
		if( strcmp(op,"jump") == 0 ) {

			fprintf(cOutputFile, "goto L_%s; ", result);
		}

		// 2.5 -> Add the original quad as a comment:
		fprintf(cOutputFile, "// (%s,%s,%s,%s)", op, arg1, arg2, result);

		fprintf(cOutputFile, "\n");

		quadsList = quadsList->next;

		// THIS IS A FAILSAFE MECHANISM TO PREVENT NEVERENDING LOOP OUTPUTS:
		failsafe++;
	}

	// Finally: Add a semicolon for the end of line and close the program brackets:
	fprintf(cOutputFile, "\n}\n");

	fclose(cOutputFile);
}
