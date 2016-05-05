
struct endcode {
	char line[128];
	struct endcode *next;
};

void gnlvcode(char name[30]);
void loadvr(char variable[30], int registerNum);
void storerv(int registerNum, char variable[30]);
void addToEndcode(char input[128]);
void endcodeGeneration(char nameoffile[30], quartet_node *quad_list_head);
void whichRelop(char *input, char *relopSign);
int isRelop(char input[30]);
void exportEndcode(char nameoffile[30], struct endcode *endcodeHead);

struct endcode *endcodeHead;
FILE *endcodeOutputFile;
int i_counter = 0; // Counter for Function Parameters

void addToEndcode(char input[128]) {

	struct endcode *current, *new_node;
	current = endcodeHead;

	new_node = malloc(sizeof(struct endcode));

	strcpy(new_node->line, input);

	if(endcodeHead == NULL) {
		endcodeHead = new_node;
		new_node->next = NULL;

	} else {

		while (current->next != NULL) {
			current = current->next;
		}

		current->next = new_node;
		new_node->next = NULL;
	}
}

void gnlvcode(char name[30]) {

	struct entity *currentEntity;
	currentEntity = lookupEntity(name);

	char gnlvOutput[128];
	sprintf(gnlvOutput, "movi R[255], M[ 4+R[0] ] ");
	addToEndcode(gnlvOutput);

	while( currentEntity->nestingLevel != scopeHead->nestingLevel ) {
		sprintf(gnlvOutput, "movi R[255], M[ 4+R[255] ]");
		addToEndcode(gnlvOutput);
		currentEntity->nestingLevel++;
	}

	sprintf(gnlvOutput, "movi R[254], %d", currentEntity->offset);
	addToEndcode(gnlvOutput);

	sprintf(gnlvOutput, "addi R[255], R[254], R[255]");
	addToEndcode(gnlvOutput);
}

void loadvr(char variable[30], int registerNum) {

	char generatedCode[128];
	struct entity *foundEntity;
	foundEntity = lookupEntity(variable);

	if(isdigit(variable[0])) {

		sprintf(generatedCode,"movi R[%d], %s", registerNum, variable);
		addToEndcode(generatedCode);

	} else if (foundEntity->nestingLevel == 1 && foundEntity->type != TEMPORARY_VARIABLE) {
		
		// Case: Global Variable
		sprintf(generatedCode, "movi R[%d], M[%d]", registerNum, 600+foundEntity->offset);
	
	} else if (foundEntity->nestingLevel == scopeHead->nestingLevel) {

		if(foundEntity->type == VARIABLE || (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_VALUE) || foundEntity->type == TEMPORARY_VARIABLE){

			sprintf(generatedCode, "movi R[%d], M[%d+R[0]]", registerNum, foundEntity->offset);
			addToEndcode(generatedCode);

		} else if (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_REFERENCE) {

			printf(generatedCode, "movi R[255], M[%d+R[0]]", foundEntity->offset);
			addToEndcode(generatedCode);
			printf(generatedCode, "movi R[%d], M[R[255]]", registerNum);
			addToEndcode(generatedCode);

		}

	} else if (foundEntity->nestingLevel < scopeHead->nestingLevel) {

		if(foundEntity->type == VARIABLE || (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_VALUE)) {

			gnlvcode(foundEntity->name);
			sprintf(generatedCode, "movi R[%d], M[R[255]]", registerNum);
			addToEndcode(generatedCode);

		} else if (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_REFERENCE) {

			gnlvcode(foundEntity->name);
			sprintf(generatedCode, "movi R[255], M[R[255]]");
			addToEndcode(generatedCode);
			sprintf(generatedCode, "movi R[%d], M[R[255]]", registerNum);
			addToEndcode(generatedCode);
		}
	}
}

void storerv(int registerNum, char variable[30]) {

	char generatedCode[128];
	struct entity *foundEntity;
	foundEntity = lookupEntity(variable);

	if (foundEntity->nestingLevel == 1 && foundEntity->type != TEMPORARY_VARIABLE) {
		
		// Case: Global Variable
		sprintf(generatedCode, "movi M[%d], R[%d]", 600+foundEntity->offset, registerNum);
		addToEndcode(generatedCode);

	} else if (foundEntity->nestingLevel == scopeHead->nestingLevel) {

		if(foundEntity->type == VARIABLE || (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_VALUE) || foundEntity->type == TEMPORARY_VARIABLE ) {

			sprintf(generatedCode, "movi M[%d+R[0]], R[%d]", foundEntity->offset, registerNum);
			addToEndcode(generatedCode);

		} else if (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_REFERENCE) {

			sprintf(generatedCode, "movi R[255], M[%d+R[0]]", foundEntity->offset);
			addToEndcode(generatedCode);
			sprintf(generatedCode, "movi M[R[255]], R[%d]", registerNum);
			addToEndcode(generatedCode);

		}

	} else if (foundEntity->nestingLevel < scopeHead->nestingLevel) {

		if (foundEntity->type == VARIABLE || (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_VALUE )) {
			
			gnlvcode(foundEntity->name);
			sprintf(generatedCode, "movi M[R[255]], R[%d]", registerNum);
			addToEndcode(generatedCode);

		} else if (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_REFERENCE) {

			gnlvcode(foundEntity->name);
			sprintf(generatedCode, "movi R[255], M[R[255]]");
			addToEndcode(generatedCode);
			sprintf(generatedCode, "movi M[R[255]], R[%d]", registerNum);
			addToEndcode(generatedCode);
		}
	}
}

void endcodeGeneration(char nameoffile[30], quartet_node *quad_list_head) {

	char generatedCode[128];
	quartet_node *current = quad_list_head;

	while(current != NULL) {

		printf("Debug: Endcode Generation.\n");

		sprintf(generatedCode, "L%d ", current->quartet.label);
		addToEndcode(generatedCode);

		if(strcmp(current->quartet.operator, "jump") == 0) {

			sprintf(generatedCode, "jmp L%s", current->quartet.result);
			addToEndcode(generatedCode);

		} else if( isRelop(current->quartet.operator) == 1 ) {

			loadvr(current->quartet.argument1, 1);
			loadvr(current->quartet.argument2, 2);
			sprintf(generatedCode, "cmpi R[1], R[2]");
			addToEndcode(generatedCode);

			char relopSign[30];
			whichRelop(current->quartet.operator, relopSign);

			sprintf(generatedCode, "%s L%s", relopSign, current->quartet.result);
			addToEndcode(generatedCode);

		} else if (strcmp(current->quartet.operator, ":=") == 0) {

			loadvr(current->quartet.argument1, 1);
			storerv(1, current->quartet.result);

		} else if (strcmp(current->quartet.operator, "+") == 0) {

			loadvr(current->quartet.argument1, 1);
			loadvr(current->quartet.argument2, 2);
			sprintf(generatedCode, "addi R[3], R[1], R[2]");
			addToEndcode(generatedCode);
			storerv(3, current->quartet.result);

		} else if (strcmp(current->quartet.operator, "-") == 0) {

			loadvr(current->quartet.argument1, 1);
			loadvr(current->quartet.argument2, 2);
			sprintf(generatedCode, "subi R[3], R[1], R[2]");
			addToEndcode(generatedCode);
			storerv(3, current->quartet.result);

		} else if (strcmp(current->quartet.operator, "*") == 0) {

			loadvr(current->quartet.argument1, 1);
			loadvr(current->quartet.argument2, 2);
			sprintf(generatedCode, "muli R[3], R[1], R[2]");
			addToEndcode(generatedCode);
			storerv(3, current->quartet.result);

		} else if (strcmp(current->quartet.operator, "/") == 0) {

			loadvr(current->quartet.argument1, 1);
			loadvr(current->quartet.argument2, 2);
			sprintf(generatedCode, "divi R[3], R[1], R[2]");
			addToEndcode(generatedCode);
			storerv(3, current->quartet.result);

		} else if (strcmp(current->quartet.operator, "out") == 0) {

			loadvr(current->quartet.argument1, 1);
			sprintf(generatedCode, "outi R[1]");
			addToEndcode(generatedCode);

		} else if (strcmp(current->quartet.operator, "in") == 0) {

			sprintf(generatedCode, "ini R[1]");
			storerv(1, current->quartet.argument1);
			addToEndcode(generatedCode);

		} else if (strcmp(current->quartet.operator, "retv") == 0) {

			loadvr(current->quartet.argument1, 1);
			sprintf(generatedCode, "movi R[255], M[8+R[0]]");
			addToEndcode(generatedCode);
			sprintf(generatedCode, "movi M[R[255]], R[1]");
			addToEndcode(generatedCode);

		} else if (strcmp(current->quartet.operator, "par") == 0) {

			struct entity *foundEntity;
			foundEntity = lookupEntity(current->quartet.argument1);

			if (strcmp(current->quartet.argument2, "CV") == 0) {

				int d = (scopeHead->framelength)+12+(4*i_counter);
				i_counter++;
				loadvr(current->quartet.argument1, 255);
				sprintf(generatedCode, "movi M[%d+R[0]], R[255]", d);
				addToEndcode(generatedCode);

			} else if (strcmp(current->quartet.argument2, "REF") == 0) {

				int d = (scopeHead->framelength)+12+(4*i_counter);
				i_counter++;

				if(foundEntity->nestingLevel == scopeHead->nestingLevel) {

					if(foundEntity->type == VARIABLE || (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_VALUE) ) {

						sprintf(generatedCode, "movi R[255], R[0]");
						addToEndcode(generatedCode);

						sprintf(generatedCode, "movi R[254], %d", foundEntity->offset);
						addToEndcode(generatedCode);

						sprintf(generatedCode, "addi R[255], R[254], R[255]");
						addToEndcode(generatedCode);

						sprintf(generatedCode, "movi M[%d+R[0]], R[255]", d);
						addToEndcode(generatedCode);

					} else if (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_REFERENCE) {

						sprintf(generatedCode, "movi R[255], R[0]");
						addToEndcode(generatedCode);

						sprintf(generatedCode, "movi R[254], %d", foundEntity->offset);
						addToEndcode(generatedCode);

						sprintf(generatedCode, "addi R[255], R[254], R[255]");
						addToEndcode(generatedCode);

						sprintf(generatedCode, "movi R[1], M[R[255]]");
						addToEndcode(generatedCode);

						sprintf(generatedCode, "movi M[%d+R[0]], R[1]", d);
						addToEndcode(generatedCode);

					}

				} else if (strcmp(current->quartet.argument2, "RET") == 0) {

				sprintf(generatedCode, "movi R[255], R[0]");
				addToEndcode(generatedCode);

				sprintf(generatedCode, "movi R[254], %d", foundEntity->offset);
				addToEndcode(generatedCode);

				sprintf(generatedCode, "movi R[255], R[254], R[255]");
				addToEndcode(generatedCode);

				sprintf(generatedCode, "movi M[%d+R[0]], R[255]", scopeHead->framelength+8);
				addToEndcode(generatedCode);

				} else {

					if(foundEntity->type == VARIABLE || (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_VALUE) ) {

						gnlvcode(foundEntity->name);
						sprintf(generatedCode, "movi M[%d+R[0]], R[255]", d);
						addToEndcode(generatedCode);

					} else if (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_REFERENCE) {

						gnlvcode(foundEntity->name);
						sprintf(generatedCode, "movi R[1], M[R[255]]");
						addToEndcode(generatedCode);
						sprintf(generatedCode, "movi M[%d+R[0]], R[1]", d);
						addToEndcode(generatedCode);

					}

				}
			}

		} else if (strcmp(current->quartet.operator, "call") == 0) {

			struct entity *foundEntity;
			foundEntity = lookupEntity(current->quartet.argument1);

			if(foundEntity->nestingLevel == scopeHead->nestingLevel) {

				sprintf(generatedCode, "movi R[255], M[4+R[0]]");
				addToEndcode(generatedCode);
				sprintf(generatedCode, "movi M[%d+R[0]], R[255]", scopeHead->framelength+4);
				addToEndcode(generatedCode);

			} else if (foundEntity->nestingLevel > scopeHead->nestingLevel) {

				sprintf(generatedCode, "movi M[%d+R[0]], R[0]", scopeHead->framelength+4);
				addToEndcode(generatedCode);

			}

			sprintf(generatedCode, "movi R[255], %d", scopeHead->framelength);
			addToEndcode(generatedCode);

			sprintf(generatedCode, "addi R[0], R[255], R[0]");
			addToEndcode(generatedCode);

			sprintf(generatedCode, "movi R[255], $");
			addToEndcode(generatedCode);

			sprintf(generatedCode, "movi R[254], 15");
			addToEndcode(generatedCode);

			sprintf(generatedCode, "addi R[255], R[255], R[254]");
			addToEndcode(generatedCode);

			sprintf(generatedCode, "movi M[R[0]], R[255]");
			addToEndcode(generatedCode);

			sprintf(generatedCode, "jmp L%d", foundEntity->function.startQuad);
			addToEndcode(generatedCode);

			sprintf(generatedCode, "movi R[255], %d", scopeHead->framelength);
			addToEndcode(generatedCode);

			sprintf(generatedCode, "subi R[0], R[0], R[255]");
			addToEndcode(generatedCode);
		
		} else if (strcmp(current->quartet.operator, "end_block") == 0) {

			sprintf(generatedCode, "jmp M[R[0]]");
			addToEndcode(generatedCode);

		} else if (strcmp(current->quartet.operator, "begin_block") == 0) {

			sprintf(generatedCode, "");
			addToEndcode(generatedCode);

		} else if (strcmp(current->quartet.operator, "halt") == 0) {

			sprintf(generatedCode, "halt");
			addToEndcode(generatedCode);

		}

		current = current->next;
	}

	// Finally, print the generated list:
	exportEndcode(nameoffile, endcodeHead);
}


int isRelop(char input[30]) {
	if(strcmp(input, "=") == 0 || strcmp(input, "<>") == 0 || strcmp(input, "<") == 0 || strcmp(input, "<=") == 0 || strcmp(input, ">") == 0 || strcmp(input, ">=") == 0 ) {
		return(1);
	} else {
		return(0);
	}
}

void whichRelop(char *input, char *relopSign) {
	
	if( strcmp(input, "=") == 0 ) {
		strcpy(relopSign, "je");
	}

	if( strcmp(input, "<>") == 0 ) {
		strcpy(relopSign, "jne");
	}

	if( strcmp(input, "<") == 0 ) {
		strcpy(relopSign, "ja");
	}

	if ( strcmp(input, "<=") == 0 ) {
		strcpy(relopSign, "jae");
	} 

	if( strcmp(input, ">") == 0 ) {
		strcpy(relopSign, "jb");
	}

	if ( strcmp(input, ">=") == 0 ) {
		strcpy(relopSign, "jbe");
	}
}

void exportEndcode(char nameoffile[30], struct endcode *endcodeHead) {

	printf("-- Starting Export of Endcode to .msim file --\n\n");

	// File Creation
	char filename[30];
	strcpy(filename, nameoffile);
	strcat(filename, ".msim");
	FILE *endcodeOutputFile = fopen(filename, "w");

	if (endcodeOutputFile == NULL) {
    	printf("Error opening file!\n");
    	exit(1);
	}

	fprintf(endcodeOutputFile, "\n");

	// Print the Node list:
	struct endcode *current = endcodeHead;

	while (current != NULL) {
		fprintf(endcodeOutputFile, "%s\n", current->line);
		current = current->next;
	}
}

