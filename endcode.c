
struct endcode {
	char line[128];
	struct endcode *next;
}

struct endcode *endcodeHead;

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

	struct entity *symbol;
	symbol = lookupEntity(name);

	char gnvlOutput[128];
	sprintf(gnvlOutput, "movi R[255], M[ 4+R[0] ] \n");
	addToEndcode(gnvlOutput);

	while( symbol->nestingLevel != scopeHead->nestingLevel ) {
		sprintf(gnlvOutput, "movi R[255], M[ 4+R[255] ] \n");
		addToEndcode(gnvlOutput);
		symbol->nestingLevel++;
	}

	sprintf(gnlvOutput, "movi R[254], offset \n");
	addToEndcode(gnvlOutput);

	sprintf(gnlvOutput, "addi R[255], R[254], R[255] \n");
	addToEndcode(gnvlOutput);
}

void loadvr(char variable[30], int registerNum) {

	char generatedCode[128];
	struct entity *foundEntity;
	foundEntity = lookupEntity(variable);

	if(isdigit(variable[0])) {

		sprintf(generatedCode,"movi R[%d], %s\n", registerNum, variable);
		addToEndcode(generatedCode);

	} else if (foundEntity->nestingLevel == 1 && foundEntity->type != TEMPORARY_VARIABLE) {
		
		// Case: Global Variable
		sprintf(generatedCode, "movi R[%d], M[%d]\n", registerNum, 600+foundEntity->offset);
	
	} else if (foundEntity->nestingLevel == scopeHead->nestingLevel) {

		if(foundEntity->type == VARIABLE || (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_VALUE) || foundEntity->type == TEMPORARY_VARIABLE){

			sprintf(generatedCode, "movi R[%d], M[%d+R[0]]\n", registerNum, foundEntity->offset);
			addToEndcode(generatedCode);

		} else if (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_REFERENCE) {

			printf(generatedCode, "movi R[255], M[%d+R[0]]\n", foundEntity->offset);
			addToEndcode(generatedCode);
			printf(generatedCode, "movi R[%d], M[R[255]]\n", registerNum);
			addToEndcode(generatedCode);

		}

	} else if (foundEntity->nestingLevel < scopeHead->nestingLevel) {

		if(foundEntity->type == VARIABLE || (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_VALUE)) {

			gnlvcode(foundEntity->name);
			sprintf(generatedCode, "movi R[%d], M[R[255]]\n", registerNum);
			addToEndcode(generatedCode);

		} else if (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_REFERENCE) {

			gnlvcode(foundEntity->name);
			sprintf(generatedCode, "movi R[255], M[R[255]]\n");
			addToEndcode(generatedCode);
			sprintf(generatedCode, "movi R[%d], M[R[255]]\n");
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
		sprintf(generatedCode, "movi M[%d], R[%d]\n", 600+foundEntity->offset, registerNum);
		addToEndcode(generatedCode);

	} else if (foundEntity->nestingLevel == scopeHead->nestingLevel) {

		if(foundEntity->type == VARIABLE || (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_VALUE) || foundEntity->type == TEMPORARY_VARIABLE ) {

			sprintf(generatedCode, "movi M[%d+R[0]], R[%d]\n", foundEntity->offset, registerNum);
			addToEndcode(generatedCode);

		} else if (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_REFERENCE) {

			sprintf(generatedCode, "movi R[255], M[%d+R[0]]\n", foundEntity->offset);
			addToEndcode(generatedCode);
			sprintf(generatedCode, "movi M[R[255]], R[%d]\n", registerNum);
			addToEndcode(generatedCode);

		}

	} else if (foundEntity->nestingLevel < scopeHead->nestingLevel) {

		if (foundEntity->type == VARIABLE || (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_VALUE )) {
			
			gnlvcode(foundEntity->name);
			sprintf(generatedCode, "movi M[R[255]], R[%d]\n", registerNum);
			addToEndcode(generatedCode);

		} else if (foundEntity->type == PARAMETER && foundEntity->parMode == PASS_BY_REFERENCE) {

			gnlvcode(foundEntity->name);
			sprintf(generatedCode, "movi R[255], M[R[255]]\n");
			addToEndcode(generatedCode);
			sprintf(generatedCode, "movi M[R[255]], R[%d]\n", registerNum);
			addToEndcode(generatedCode);
		}
	}
}


