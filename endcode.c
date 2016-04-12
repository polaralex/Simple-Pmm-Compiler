
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