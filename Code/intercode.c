// Quartet Structs:
struct quad
{
	int label;
	char operator[30];
	char argument1[30];
	char argument2[30];
	char result[30];
} quartet;

// Linked-List Structs:
typedef struct label_list {
	int label;
	struct quarter_list *next;
} label_node;

typedef struct quartet_list {
	struct quad quartet;
	struct quartet_list *next;
} quartet_node;

// Helper Subroutines:
int nextquad();
void genquad(char *operator, char *x, char *y, char *z);
char * newtemp();

label_list * emptylist();
label_list * makelist(int quad_label);
void merge(label_list * list1, label_list * list2);
void backpatch(label_list list, int z);

int nextquad() {

	nextquad = nextquad + 10;
	return(nextquad);
}

void genquad(char *operator, char *x, char *y, char *z) {

	// Generate a Quad here, and add it in the Quad list.
}

// Creates and Returns a Temporary variable (in the format of "T_1", "T_2" etc.):
char * newtemp() {

	char *temp = (char *) malloc (sizeof(30));
	sprintf( temp, "T_%d", temp_number);

	temp_number = temp_number + 1;

	return temp;
}

// Creates and Returns an Empty List of Quads:
label_list * emptylist() {

	label_list *temp_list = NULL;
	return temp_list;
}

// Creates and Returns a List of Quad-Labels with one Label element:
label_list * makelist(int quad_label) {

	label_list *temp_list = malloc(sizeof(label_node));
	temp_list -> label = quad_label;
	temp_list -> next = NULL;

	return temp_list;
}

// Merges two Lists of Quad-Labels and Returns the Sorted List Result:
label_list * merge (label_list *list1, label_list * list2) {

	// Posibility of one list being NULL:
	if ( !list1 ) {
		return(list2);
	}

	if ( !list2 ) {
		return(list1);
	}

	label_list * temp_list;

	// Swap the lists if list2's label is smaller that list1's.
	// In the end, we will hold the smallest value in list1:
	if ( (list1 -> label) > (list2 -> label) ) {
		temp_list = list2;
		list2 = list1;
		list1 = temp_list;
	}

	label_list *tail = list1;

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

void backpatch(label_list list, int z) {

	quartet_node *current = head; //THIS???
	label_list *temp = list;

	while (current != NULL && temp != NULL) {

		if ( temp->label == current->quartet.label ) {
			sprintf (current->quartet.result, "%d", z);
			temp = temp->next;
		}

		current = current->next;
	}
}
