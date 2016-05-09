
#define VARIABLE_E 1
#define FUNCTION 2
#define CONSTANT 3
#define PARAMETER 4
#define TEMPORARY_VARIABLE 5

#define PASS_BY_VALUE 6
#define PASS_BY_REFERENCE 7

#define TYPE_FUNCTION 8
#define TYPE_PROCEDURE 9

struct entity {

	char name[30];
	int nestingLevel;
	struct entity *next;

	// Variable (and "offset" used for Parameter):
	int type;
	int offset;

	// Function:
	int startQuad;
	struct recordArgument *argumentList;

	// Constant:
	char value[30];

	// Parameter / tempParameter:
	int parMode;
};

struct scope {
	char name[30];
	int nestingLevel;
	struct entity *entityList;
	int framelength;
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

void addScope(char name[30]);
void deleteScope();
void addEntity(char name[30], int type, int mode, char value[30]);
struct entity *lookupEntity(char name[30]);
void printSymbolTable();

// Functions:
void addScope(char name[30]) {

	printf("Debug: Inside addScope.\n\n");

	// This function pushes a new_scope to the top of the scope stack:
	struct scope *new_scope;
	new_scope = malloc(sizeof(struct scope));

	strcpy(new_scope->name, name);
	new_scope->entityList = NULL;

	if (scopeHead == NULL) {
		scopeHead = new_scope;
		new_scope->nestingLevel = 1;
		new_scope->framelength = 12;
		new_scope->next = NULL;
	} else {
		new_scope->nestingLevel = scopeHead->nestingLevel + 1;
		new_scope->framelength = 12;
		new_scope->next = scopeHead;
		scopeHead = new_scope;
	}
}

void deleteScope() {

	printf("Debug: Inside deleteScope.\n\n");

	if (scopeHead == NULL) {
		printf("The scope stack is empty.\n\n");
		exit(1);
	} else {
		scopeHead = scopeHead->next;
	}
}

void addEntity(char name[30], int type, int mode, char value[30]) {

	printf("Debug: Inside addEntity.\n\n");

	struct entity *new_entity;
	struct entity *current;
	struct entity *previous;

	int last_offset = -1;

	new_entity = malloc(sizeof(struct entity));

	// Data copy:
	strcpy(new_entity->name, name);
	new_entity->type = type;
	new_entity->parMode = mode;
	strcpy(new_entity->value, value);
	new_entity->nestingLevel = scopeHead->nestingLevel;

	new_entity->next = NULL;

	previous = NULL;
	current = scopeHead->entityList;

	if(current == NULL) {

		scopeHead->entityList = new_entity;
		new_entity->offset = 12;
		scopeHead->framelength = 16;

	} else {

		while (current != NULL){

			previous = current;

			if(current->type != TYPE_FUNCTION || current->type != TYPE_PROCEDURE) {
				last_offset = current->offset;
			}

			current = current->next;
		}

		previous->next = new_entity;
		new_entity->offset = last_offset + 4;
		scopeHead->framelength = new_entity->offset + 4;
	}
}

struct entity *lookupEntity(char name[30]) {

	printf("Debug: Inside lookupEntity.\n\n");

	struct entity *currentEntity;
	struct scope *currentScope;
	currentScope = scopeHead;

	while (currentScope != NULL) {

		currentEntity = currentScope->entityList;

		while (currentEntity != NULL) {

			if(strcmp(currentEntity->name, name) == 0 || strcmp(currentScope->name, name) == 0) {
				
				return currentEntity;
			}
			
			currentEntity = currentEntity->next;
		}

		currentScope=currentScope->next;
	}

	if (!isdigit(name[0])) {
		printf("The requested Entity (%s) was not found.\n\n", name);
	}

	// Returns NULL if the Entity is not found:
	return(NULL);
}

void printSymbolTable() {

	struct scope *currentScope;
	struct entity *currentEntity;

	currentScope = scopeHead;

	printf("-- Symbol Table Printout --\n");

	while(currentScope != NULL) {
		
		printf("\nNesting Level: #%d\n", currentScope->nestingLevel);
		printf("Scope Name: %s\n\n", currentScope->name);

		currentEntity = currentScope->entityList;

		while (currentEntity != NULL) {

			if(currentEntity->type == VARIABLE_E) {
				printf("<Variable %s, %d> \n", currentEntity->name, currentEntity->offset);
			} else if (currentEntity->type == TYPE_FUNCTION) {
				printf("<Function: %s> \n", currentEntity->name);
			} else if (currentEntity->type == TYPE_PROCEDURE) {
				printf("<Procedure: %s> \n", currentEntity->name);
			} else if (currentEntity->type == CONSTANT) {
				printf("<Constant: %s, %d, Value:%s> \n", currentEntity->name, currentEntity->offset, currentEntity->value);
			} else if (currentEntity->type == PARAMETER) {
				printf("<Parameter: %s, %d> \n", currentEntity->name, currentEntity->offset);
			} else {
				// Case: Temporary Variable
				printf("<Temporary Variable: %s, %d> \n", currentEntity->name, currentEntity->offset);
			}

			currentEntity = currentEntity->next;
		}

		currentScope = currentScope->next;
	}

	printf("\n-- End of Symbol Table --\n\n");
}
