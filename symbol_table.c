#include "defines.h"

#define VARIABLE_E 1
#define FUNCTION 2
#define CONSTANT 3
#define PARAMETER 4
#define TEMPORARY_VARIABLE 5

// Data Structs:

// Γιατί χρειάζεται αυτό;
struct functionType {
	int startQuad;
	int type;
	char name[30];
	struct recordArgument *argumentList;
};

struct entity {
	char name[30];
	// Variable (and "offset" used for Parameter):
	int type;
	int offset;
	// Function:
	struct functionType function;
	int startQuad;
	struct argument *argumentList;
	int framelength;
	// Constant:
	char value[30];
	// Parameter / tempParameter:
	int parMode;

	int nestingLevel;
	struct entity *next;
};

struct scope {
	struct entity *entityList;
	char name[30];
	int nestingLevel;
	int framelength; // ΤΣΕΚΑΡΕ ΑΥΤΟ. ΓΙΑΤΙ ΕΙΝΑΙ ΕΔΩ;
	struct scope *next;
};

struct recordArgument {
	int parMode;
	int type;
	struct recordArgument *next;
};

// List Global Variables:
struct scope *scopeHead = NULL;
struct recordArgument *argumentHead = NULL;

// Functions:

void addScope(char name[30]) {

	// This function pushes a new_scope to the top of the scope stack:

	struct scope *new_scope;
	new_scope = malloc(sizeof(struct scope));

	strcpy(new_scope->name, name);

	new_scope->entityList = NULL;

	if (scopeHead == NULL) {
		scopeHead = new_scope;
		new_scope->nestingLevel=1;
		new_scope->framelength=12;
		new_scope->next = NULL;
	} else {
		new_scope->nestingLevel= scopeHead->nestingLevel+1;
		new_scope->framelength=12;
		new_scope->next = scopeHead;
		scopeHead = new_scope;
	}
}

void deleteScope() {

	if (scopeHead == NULL) {
		printf("The scope stack is empty.\n\n");
		exit(1);
	} else {
		scopeHead = scopeHead->next;
	}
}

void addEntity(char name[30], int type, int mode, char value[30]) {

	struct entity *new_entity;
	struct entity *current;
	struct entity *previous;

	int temp_offset = -1;

	new_entity = malloc(sizeof(struct entity));

	if(type == FUNCTION){
		strcpy(new_entity->function.name, name);
	} else {
		strcpy(new_entity->name, name);
	}

	new_entity->type = type;
	new_entity->parMode = mode;
	strcpy(new_entity->value, value);
	new_entity->nestingLevel = scopeHead->nestingLevel;
	new_entity->next = NULL;

	previous = NULL;
	current = scopeHead->entityList;

	while (current != NULL){
		previous = current;

		if(current->type != FUNCTION) {
			temp_offset = current->offset;
		}
		current = current->next;
	}

	if (previous == NULL) {
		scopeHead->entityList = new_entity;
		new_entity->offset = 12;
		scopeHead->framelength = 16;
	} else {
		previous->next = new_entity;
		new_entity->offset = temp_offset + 4;
		scopeHead->framelength = new_entity->offset + 4;
	}
}

struct entity *lookupEntity(char name[30]) {

	struct entity *currentEntity;
	struct scope *currentScope;
	currentScope = scopeHead;

	while (currentScope != NULL) {
		currentEntity = currentScope->entityList;

		while (currentEntity != NULL) {

			if(strcmp(currentEntity->name, name) == 0 == strcmp(currentEntity->function.name, name)==0 || strcmp(currentScope->name, name) == 0) {
				return currentEntity;
			} else {
				currentEntity = currentEntity->next;
			}
		}

		currentScope=currentScope->next;
	}

	if (!isdigit(name[0])) {
		printf("The requested Entity (%s) was not found.\n\n", name);
	}

	return(NULL);
}

void printSymbolTable() {

	struct scope *currentScope;
	struct entity *currentEntity;

	currentScope = scopeHead;

	printf("-- Symbol Table Printout --\n\n");

	while(currentScope != NULL) {
		
		printf("Nesting Level: #%d\n", currentScope->nestingLevel);
		printf("Scope Name: %s\n\n", currentScope->name);

		currentEntity = currentScope->entityList;

		while (currentEntity != NULL) {

			if(currentEntity->type == VARIABLE_E) {
				printf("<Variable %s, %d> \n", currentEntity->name, currentEntity->offset);
			} else if (currentEntity->type == FUNCTION) {
				printf("<Function: %s> \n", currentEntity->function.name);
			} else if (currentEntity->type == CONSTANT) {
				printf("<Constant: %s, %d, Value:%s> \n", currentEntity->name, currentEntity->offset, currentEntity->value);
			} else if (currentEntity->type == PARAMETER) {
				printf("<Parameter: %s, %d> \n", currentEntity->name, currentEntity->offset);
			} else {
				// Case: Temporary Variable
				printf("<Temporary Variable: %s, %d> \n", currentEntity->name, currentEntity->offset);
			}
		}

		printf("\n\n");
		currentScope = currentScope->next;

	}
}
