
struct endcode {
	char line[128];
	struct endcode *next;
};

void gnlvcode(char name[30]);
void loadvr(char variable[30], int registerNum);
void storerv(int registerNum, char variable[30]);
void addToEndcode(char input[128]);
void endcodeGeneration();
void whichRelop(char *input, char *relopSign);
int isRelop(char input[30]);
void exportEndcode(struct endcode *endcodeHead);
void debugPrintEndcode(struct endcode *endcodeHead);

struct endcode *endcodeHead;
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

	debugPrintEndcode(endcodeHead);
}

void gnlvcode(char name[30]) {

	struct entity *currentEntity;
	currentEntity = lookupEntity(name);

	char gnlvOutput[128];
	sprintf(gnlvOutput, "\tmovi R[255], M[4+R[0]]\n");
	addToEndcode(gnlvOutput);

	while( currentEntity->nestingLevel != scopeHead->nestingLevel ) {
		sprintf(gnlvOutput, "\tmovi R[255], M[4+R[255]]\n");
		addToEndcode(gnlvOutput);
		currentEntity->nestingLevel++;
	}

	sprintf(gnlvOutput, "\tmovi R[254], %d\n", currentEntity->offset);
	addToEndcode(gnlvOutput);

	sprintf(gnlvOutput, "\taddi R[255], R[254], R[255]\n");
	addToEndcode(gnlvOutput);
}

void loadvr(char variable[30], int registerNum) {

	printf("Inside loadvr: START\n\n");

	char generatedCode[128];

	struct entity *foundEntity;
	foundEntity = lookupEntity(variable);

	if(isdigit(variable[0])) {

		printf("Inside loadvr: 1\n\n");

		sprintf(generatedCode,"\tmovi R[%d], %s\n", registerNum, variable);
		addToEndcode(generatedCode);

	} else if (foundEntity->nestingLevel == 1 && foundEntity->type != TEMPORARY_VARIABLE) {
		
		printf("Inside loadvr: 2\n\n");

		// Case: Global Variable
		sprintf(generatedCode, "\tmovi R[%d], M[%d]\n", registerNum, 600+foundEntity->offset);
		addToEndcode(generatedCode);
	
	} else if (foundEntity->nestingLevel == scopeHead->nestingLevel) {
		// Entity resides in current scope:

		printf("Inside loadvr: 3\n\n");

		if(foundEntity->type == VARIABLE_E || (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_VALUE) || foundEntity->type == TEMPORARY_VARIABLE){

			sprintf(generatedCode, "\tmovi R[%d], M[%d+R[0]]\n", registerNum, foundEntity->offset);
			addToEndcode(generatedCode);

		} else if (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_REFERENCE) {

			printf(generatedCode, "\tmovi R[255], M[%d+R[0]]\n", foundEntity->offset);
			addToEndcode(generatedCode);

			printf(generatedCode, "\tmovi R[%d], M[R[255]]\n", registerNum);
			addToEndcode(generatedCode);

		}

	} else if (foundEntity->nestingLevel < scopeHead->nestingLevel) {

		printf("Inside loadvr: 4\n\n");

		if(foundEntity->type == VARIABLE_E || (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_VALUE)) {

			gnlvcode(foundEntity->name);
			sprintf(generatedCode, "\tmovi R[%d], M[R[255]]\n", registerNum);
			addToEndcode(generatedCode);

		} else if (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_REFERENCE) {

			gnlvcode(foundEntity->name);
			sprintf(generatedCode, "\tmovi R[255], M[R[255]]\n");
			addToEndcode(generatedCode);

			sprintf(generatedCode, "\tmovi R[%d], M[R[255]]\n", registerNum);
			addToEndcode(generatedCode);
		}
	}
}

void storerv(int registerNum, char variable[30]) {

	printf("Inside StoreRV: GENERAL\n");

	char generatedCode[128];

	struct entity *foundEntity;
	foundEntity = lookupEntity(variable);

	if (foundEntity->nestingLevel == 1 && foundEntity->type != TEMPORARY_VARIABLE) {

		printf("Inside StoreRV: 1\n");
		
		// Case: Global Variable
		sprintf(generatedCode, "\tmovi M[%d], R[%d]\n", 600+foundEntity->offset, registerNum);
		addToEndcode(generatedCode);

	} else if (foundEntity->nestingLevel == scopeHead->nestingLevel) {

		printf("Inside StoreRV: 2\n");

		if(foundEntity->type == VARIABLE_E || (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_VALUE) || foundEntity->type == TEMPORARY_VARIABLE ) {

			printf("Inside StoreRV: 2->1\n");

			sprintf(generatedCode, "\tmovi M[%d+R[0]], R[%d]\n", foundEntity->offset, registerNum);
			addToEndcode(generatedCode);

		} else if (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_REFERENCE) {

			printf("Inside StoreRV: 2->2\n");

			sprintf(generatedCode, "\tmovi R[255], M[%d+R[0]]\n", foundEntity->offset);
			addToEndcode(generatedCode);

			sprintf(generatedCode, "\tmovi M[R[255]], R[%d]\n", registerNum);
			addToEndcode(generatedCode);
		}

	} else if (foundEntity->nestingLevel < scopeHead->nestingLevel) {

		printf("Inside StoreRV: 3\n");

		if (foundEntity->type == VARIABLE_E || (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_VALUE )) {
			
			printf("Inside StoreRV: 3->1\n");

			gnlvcode(foundEntity->name);
			sprintf(generatedCode, "\tmovi M[R[255]], R[%d]\n", registerNum);
			addToEndcode(generatedCode);

		} else if (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_REFERENCE) {

			printf("Inside StoreRV: 3->2\n");

			gnlvcode(foundEntity->name);
			sprintf(generatedCode, "\tmovi R[255], M[R[255]]\n");
			addToEndcode(generatedCode);

			sprintf(generatedCode, "\tmovi M[R[255]], R[%d]\n", registerNum);
			addToEndcode(generatedCode);
		}
	}
}

void endcodeGeneration() {

	printf("Debug: Endcode Generation.\n");

	char generatedCode[128];

	// This keeps the print pointer to the same place, in between
	// changes of scopes:
	if(checkForEndcodeQuadPointerIfAlreadyInitialized == 0){
		currentEndcode = quad_list_head;
		checkForEndcodeQuadPointerIfAlreadyInitialized = 1;
	} else {
		currentEndcode = previousCurrentEndcode;
		currentEndcode = currentEndcode->next;
	}

	while (currentEndcode != NULL) {

		printf("Debug: Endcode Generation LOOP.\n");

		sprintf(generatedCode, "L%d \n", currentEndcode->quartet.label);
		addToEndcode(generatedCode);

		if(strcmp(currentEndcode->quartet.operator, "jump") == 0) {

			sprintf(generatedCode, "\tjmp L%s\n", currentEndcode->quartet.result);
			addToEndcode(generatedCode);

		} else if( isRelop(currentEndcode->quartet.operator) == 1 ) {

			loadvr(currentEndcode->quartet.argument1, 1);
			loadvr(currentEndcode->quartet.argument2, 2);
			sprintf(generatedCode, "\tcmpi R[1], R[2]\n");
			addToEndcode(generatedCode);

			char relopSign[30];
			whichRelop(currentEndcode->quartet.operator, relopSign);

			sprintf(generatedCode, "\t%s L%s\n", relopSign, currentEndcode->quartet.result);
			addToEndcode(generatedCode);

		} else if (strcmp(currentEndcode->quartet.operator, ":=") == 0) {

			loadvr(currentEndcode->quartet.argument1, 1);
			storerv(1, currentEndcode->quartet.result);

		} else if (strcmp(currentEndcode->quartet.operator, "+") == 0) {

			loadvr(currentEndcode->quartet.argument1, 1);
			loadvr(currentEndcode->quartet.argument2, 2);
			sprintf(generatedCode, "\taddi R[3], R[1], R[2]\n");
			addToEndcode(generatedCode);
			storerv(3, currentEndcode->quartet.result);

		} else if (strcmp(currentEndcode->quartet.operator, "-") == 0) {

			loadvr(currentEndcode->quartet.argument1, 1);
			loadvr(currentEndcode->quartet.argument2, 2);
			sprintf(generatedCode, "\tsubi R[3], R[1], R[2]\n");
			addToEndcode(generatedCode);
			storerv(3, currentEndcode->quartet.result);

		} else if (strcmp(currentEndcode->quartet.operator, "*") == 0) {

			loadvr(currentEndcode->quartet.argument1, 1);
			loadvr(currentEndcode->quartet.argument2, 2);
			sprintf(generatedCode, "\tmuli R[3], R[1], R[2]\n");
			addToEndcode(generatedCode);
			storerv(3, currentEndcode->quartet.result);

		} else if (strcmp(currentEndcode->quartet.operator, "/") == 0) {

			loadvr(currentEndcode->quartet.argument1, 1);
			loadvr(currentEndcode->quartet.argument2, 2);
			sprintf(generatedCode, "\tdivi R[3], R[1], R[2]\n");
			addToEndcode(generatedCode);
			storerv(3, currentEndcode->quartet.result);

		} else if (strcmp(currentEndcode->quartet.operator, "out") == 0) {

			loadvr(currentEndcode->quartet.argument1, 1);
			sprintf(generatedCode, "\touti R[1]\n");
			addToEndcode(generatedCode);

		} else if (strcmp(currentEndcode->quartet.operator, "in") == 0) {

			sprintf(generatedCode, "\tini R[1]\n");
			storerv(1, currentEndcode->quartet.argument1);
			addToEndcode(generatedCode);

		} else if (strcmp(currentEndcode->quartet.operator, "retv") == 0) {

			loadvr(currentEndcode->quartet.argument1, 1);
			sprintf(generatedCode, "\tmovi R[255], M[8+R[0]]\n");
			addToEndcode(generatedCode);
			sprintf(generatedCode, "\tmovi M[R[255]], R[1]\n");
			addToEndcode(generatedCode);

		} else if (strcmp(currentEndcode->quartet.operator, "par") == 0) {

			struct entity *foundEntity;
			foundEntity = lookupEntity(currentEndcode->quartet.argument1);

			if (strcmp(currentEndcode->quartet.argument2, "CV") == 0) {

				int d = (scopeHead->framelength)+12+(4*i_counter);
				i_counter++;
				loadvr(currentEndcode->quartet.argument1, 255);
				sprintf(generatedCode, "\tmovi M[%d+R[0]], R[255]\n", d);
				addToEndcode(generatedCode);

			} else if (strcmp(currentEndcode->quartet.argument2, "REF") == 0) {

				int d = (scopeHead->framelength)+12+(4*i_counter);
				i_counter++;

				if(foundEntity->nestingLevel == scopeHead->nestingLevel) {

					if(foundEntity->type == VARIABLE_E || (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_VALUE) ) {

						sprintf(generatedCode, "\tmovi R[255], R[0]\n");
						addToEndcode(generatedCode);

						sprintf(generatedCode, "\tmovi R[254], %d\n", foundEntity->offset);
						addToEndcode(generatedCode);

						sprintf(generatedCode, "\taddi R[255], R[254], R[255]\n");
						addToEndcode(generatedCode);

						sprintf(generatedCode, "\tmovi M[%d+R[0]], R[255]\n", d);
						addToEndcode(generatedCode);

					} else if (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_REFERENCE) {

						sprintf(generatedCode, "\tmovi R[255], R[0]\n");
						addToEndcode(generatedCode);

						sprintf(generatedCode, "\tmovi R[254], %d\n", foundEntity->offset);
						addToEndcode(generatedCode);

						sprintf(generatedCode, "\taddi R[255], R[254], R[255]\n");
						addToEndcode(generatedCode);

						sprintf(generatedCode, "\tmovi R[1], M[R[255]]\n");
						addToEndcode(generatedCode);

						sprintf(generatedCode, "\tmovi M[%d+R[0]], R[1]\n", d);
						addToEndcode(generatedCode);

					}

				} else if (strcmp(currentEndcode->quartet.argument2, "RET") == 0) {

				sprintf(generatedCode, "\tmovi R[255], R[0]\n");
				addToEndcode(generatedCode);

				sprintf(generatedCode, "\tmovi R[254], %d\n", foundEntity->offset);
				addToEndcode(generatedCode);

				sprintf(generatedCode, "\tmovi R[255], R[254], R[255]\n");
				addToEndcode(generatedCode);

				sprintf(generatedCode, "\tmovi M[%d+R[0]], R[255]\n", scopeHead->framelength+8);
				addToEndcode(generatedCode);

				} else {

					if(foundEntity->type == VARIABLE_E || (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_VALUE) ) {

						gnlvcode(foundEntity->name);
						sprintf(generatedCode, "\tmovi M[%d+R[0]], R[255]\n", d);
						addToEndcode(generatedCode);

					} else if (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_REFERENCE) {

						gnlvcode(foundEntity->name);
						sprintf(generatedCode, "\tmovi R[1], M[R[255]]\n");
						addToEndcode(generatedCode);
						sprintf(generatedCode, "\tmovi M[%d+R[0]], R[1]\n", d);
						addToEndcode(generatedCode);

					}

				}
			}

		} else if (strcmp(currentEndcode->quartet.operator, "call") == 0) {

			struct entity *foundEntity;
			foundEntity = lookupEntity(currentEndcode->quartet.argument1);

			if(foundEntity->nestingLevel == scopeHead->nestingLevel) {

				sprintf(generatedCode, "\tmovi R[255], M[4+R[0]]\n");
				addToEndcode(generatedCode);
				sprintf(generatedCode, "\tmovi M[%d+R[0]], R[255]\n", scopeHead->framelength+4);
				addToEndcode(generatedCode);

			} else if (foundEntity->nestingLevel > scopeHead->nestingLevel) {

				sprintf(generatedCode, "\tmovi M[%d+R[0]], R[0]\n", scopeHead->framelength+4);
				addToEndcode(generatedCode);

			}

			sprintf(generatedCode, "\tmovi R[255], %d\n", scopeHead->framelength);
			addToEndcode(generatedCode);

			sprintf(generatedCode, "\taddi R[0], R[255], R[0]\n");
			addToEndcode(generatedCode);

			sprintf(generatedCode, "\tmovi R[255], $\n");
			addToEndcode(generatedCode);

			sprintf(generatedCode, "\tmovi R[254], 15\n");
			addToEndcode(generatedCode);

			sprintf(generatedCode, "\taddi R[255], R[255], R[254]\n");
			addToEndcode(generatedCode);

			sprintf(generatedCode, "\tmovi M[R[0]], R[255]\n");
			addToEndcode(generatedCode);

			sprintf(generatedCode, "\tjmp L%d\n", foundEntity->startQuad);
			addToEndcode(generatedCode);

			sprintf(generatedCode, "\tmovi R[255], %d\n", scopeHead->framelength);
			addToEndcode(generatedCode);

			sprintf(generatedCode, "\tsubi R[0], R[0], R[255]\n");
			addToEndcode(generatedCode);
		
		} else if (strcmp(currentEndcode->quartet.operator, "end_block\n") == 0) {

			sprintf(generatedCode, "\tjmp M[R[0]]\n");
			addToEndcode(generatedCode);

		} else if (strcmp(currentEndcode->quartet.operator, "begin_block\n") == 0) {

			sprintf(generatedCode, "\t\n");
			addToEndcode(generatedCode);

		} else if (strcmp(currentEndcode->quartet.operator, "halt\n") == 0) {

			sprintf(generatedCode, "\thalt\n");
			addToEndcode(generatedCode);

		}

		previousCurrentEndcode = currentEndcode;
		currentEndcode = currentEndcode->next;
	}

	// Finally, print the generated list:
	printf("[ENDCODE] Exporting Endcode to File.\n\n");
	exportEndcode(endcodeHead);
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

void debugPrintEndcode(struct endcode *endcodeHead) {

	// Print the Node list:
	struct endcode *current = endcodeHead;

	printf("\n--Debug Endcode Print--\n");
	while (current != NULL) {
		printf("%s", current->line);
		current = current->next;
	}
	printf("\n-- End of Debug Endcode--\n\n");
}

void exportEndcode(struct endcode *endcodeHead) {

	printf("-- Starting Export of Endcode to .msim file --\n\n");

	if (endcodeOutputFile == NULL) {
    	printf("Error opening file!\n");
    	exit(1);
	}

	fprintf(endcodeOutputFile, "\n");

	// Print the Node list:
	struct endcode *current = endcodeHead;

	while (current != NULL) {
		fprintf(endcodeOutputFile, "%s", current->line);
		current = current->next;
	}
}
