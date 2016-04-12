#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "defines.h"
#include "intercode_functions.c"
#include "symbol_table.c"

// P-- Grammar:
void program();
void block(char function_name[30], int isItMainBlock);
void declarations();
void expression(char **E_place);
void brack_or_stat();
void formalParItem();
void statement();
void actualpars();
void actualparlist();
void actualparitem();
void boolterm(label_node **Q_True, label_node **Q_False);
void boolFactor(label_node **R_True, label_node **R_False);
void factor(char **F_Place);
void mul_oper();
void idtail(char **Id_Place);
void elsepart();
void varlist();
void relational_oper();
void optionalSign();
void term(char **T_Place);
void add_oper();
void subprograms();
void sequence();
void condition(label_node **B_True, label_node **B_False);
void funcBody(char procedure_name[30]);
void func();
void formalPars();
void formalParList();
void step_part();
void brackets_seq();

// Helper Functions:
void getNextToken();

/* ------------------------------------------------- */
/*      Beginning of Syntax Analyzer Functions:      */
/* ------------------------------------------------- */

void program() {

	printf("Syntax Debug: Inside program.\n\n");
	printf("Token is %d (peeked is %d)\n", token, peekToken);

	if( token == program_a ){

		getNextToken();

		if ( token == VARIABLE ) {

			char function_name[30];
			strcpy(function_name, currentLexeme);

			addScope(function_name);

			block(function_name, IS_MAIN_PROGRAM);

			printSymbolTable();

			deleteScope();

		} else {
			error("Program name expected.");
		}

	} else {
		error("The keyword 'program' was expected.");
	}
}

void block(char function_name[30], int isItMainBlock) {

	printf("Syntax Debug: Inside Block.\n\n");

	getNextToken();

	if (token == curlbrackleft) {

		declarations();

		while ( peekToken == procedure_a || peekToken == function_a ) {
			subprograms();
		}

		genquad("begin_block", function_name, "_", "_");

		sequence();

		if ( isItMainBlock == 1 ) {
			genquad("halt", "_", "_", "_");
		}

		genquad("end_block", function_name, "_", "_");

		getNextToken();

		if( token != curlbrackright ) {
			error("A Closing Curly Bracket was expected at the end of program/function block.");
		}
	}
}

void declarations() {

	printf("Syntax Debug: Inside declarations.\n\n");

	if (peekToken == var_a) {

		// Consume 'var':
		getNextToken();

		varlist();

		getNextToken();

		if (token != semicolon) {
			error("Semicolon needed at the end of variable declarations.");
		}

		printf("Syntax Debug: Variable Declarations ended with no problems.\n\n");
	}
}

void varlist() {

	printf("Syntax Debug: Inside varlist.\n\n");

	if (peekToken == VARIABLE) {

		// Consume the 'var':
		getNextToken();

		addEntity(currentLexeme, VARIABLE_E, 0, "0");

		flagLexemeAsUsed();

		while ( peekToken == comma || peekToken == VARIABLE ) {

			if ( peekToken == comma ) {

				// Consume the 'comma':
				getNextToken();

				if ( peekToken != VARIABLE ) {

					error("Excess commas found in variable declarations.");

				} else {

					getNextToken();

					addEntity(currentLexeme, VARIABLE_E, 0, "0");

					flagLexemeAsUsed();

				}

			} else {

				error("Comma needed between variable declarations.");
			}
		}
	} else {

		error("Variable declaration(s) needed after 'var' keyword.");
	}
}

void elsepart(){

	printf("Syntax Debug: Inside elsepart.\n\n");

	if ( peekToken == else_a) {

		// Consume the "else" token:
		getNextToken();

		brack_or_stat();

	}
}

void boolFactor(label_node **R_True, label_node **R_False) {

	printf("Syntax Debug: Inside boolFactor.\n\n");

	// Case 1:
	if (peekToken == not_a) {

		// Consume 'not' token:
		getNextToken();

		getNextToken();

		if (token == bracketleft) {

			label_node *B_True;
			label_node *B_False;

			condition(&B_True, &B_False);

			// {P1}:
			*R_True = B_False;
			*R_False = B_True;

			getNextToken();

			if (token != bracketright) {
				error("Right Bracket expected in closing of 'boolFactor'.");
			}

		} else {
			error("Opening Bracket needed after 'not' statement.");
		}

	// Case 2:
	} else if ( peekToken == bracketleft ) {

		// Consume 'left bracket Token'
		getNextToken();

		label_node *B_True;
		label_node *B_False;

		condition(&B_True, &B_False);

		// {P1}:
		*R_True = B_True;
		*R_False = B_False;

		getNextToken();

		if ( token != bracketright ) {
			error("Right Bracket expected after Opening Bracket.");
		}

	// Case 3:
	} else {

		// E1:
		char* E1_Place = malloc(sizeof(char)*30);
		expression(&E1_Place);

		printf("Debug: Before calling 'relational_operator', the token is: %d\n", token);

		// relop:
		char relational_operator[10];
		relational_oper(relational_operator);

		// E2:
		char* E2_Place = malloc(sizeof(char)*30);
		expression(&E2_Place);

		// {P1}:
		*R_True = makelist(nextquad());
		
		genquad(relational_operator, E1_Place, E2_Place, "_");

		*R_False = makelist(nextquad());

		genquad("jump","_","_","_");
	}
}

void expression(char **E_place) {

	printf("Syntax Debug: Inside expression.\n\n");

	optionalSign();

	char *T1_Place;
	T1_Place = malloc(sizeof(char)*30);

	term(&T1_Place);

	while (peekToken == plus || peekToken == minus) {

		add_oper();

		if ( token == plus ) {

			char *T2_Place;
			T2_Place = malloc(sizeof(char)*30);

			term(&T2_Place);

			// {P1}:
			char *w = newtemp();

			addEntity(w, 5, 0, "0");

			genquad("+", T1_Place, T2_Place, w);
			strcpy(T1_Place, w);

		} else if ( token == minus ) {

			char *T2_Place;
			T2_Place = malloc(sizeof(char)*30);

			term(&T2_Place);

			// {P1}:
			char *w = newtemp();

			addEntity(w, 5, 0, "0");

			genquad("-", T1_Place, T2_Place, w);
			strcpy(T1_Place, w);
		}
	}

	// {P2}:
	*E_place = malloc(sizeof(30));
	strcpy(*E_place, T1_Place);
}

void subprograms() {

	printf("Syntax Debug: Inside Subprograms.\n\n");

	while ( peekToken == procedure_a || peekToken == function_a ) {
		func();
	}
}


void func() {

	printf("Syntax Debug: Inside func.\n\n");

	getNextToken();

	char procedure_name[30];

	if (token == procedure_a) {

		getNextToken();

		if (token == VARIABLE) {

			strcpy(procedure_name, currentLexeme);

			addEntity(procedure_name, FUNCTION, 0, "0");

			struct entity *func_entity;
			func_entity=lookupEntity(procedure_name);
			func_entity->function.startQuad = nextquadlabel;
			func_entity->function.type = TYPE_PROCEDURE;

			addScope(procedure_name);

			funcBody(procedure_name);

			printSymbolTable();

			deleteScope();
		}

	} else if (token == function_a) {

		getNextToken();

		if (token == VARIABLE) {

			strcpy(procedure_name, currentLexeme);

			addEntity(procedure_name, FUNCTION, 0, "0");

			struct entity *func_entity;
			func_entity=lookupEntity(procedure_name);
			func_entity->function.startQuad = nextquadlabel;
			func_entity->function.type = TYPE_FUNCTION;

			addScope(procedure_name);
			
			funcBody(procedure_name);

			printSymbolTable();

			deleteScope();
		}

	} else {
		error("Function declaration (procedure or function) needed.");
	}
}

void funcBody(char procedure_name[30]) {

	printf("Syntax Debug: Inside funcBody.\n\n");

	formalPars();
	block(procedure_name, 0);
}

void formalPars() {

	printf("Syntax Debug: Inside formalPars.\n\n");

	getNextToken();

	if ( token == parenthleft ) {

		if ( peekToken == in_a || peekToken == inout_a ) {

			formalParList();

			getNextToken();

			if ( token != parenthright ) {
				error("No closing parenthesis found after opening for Parameters definition.");
			}

		} else {

			getNextToken();

			if ( token != parenthright ) {
				error("No closing parenthesis found after opening for Parameters definition.");
			}
		}
	}
}

void formalParList() {

	printf("Syntax Debug: Inside formalParList.\n\n");

	formalParItem();

	while ( peekToken == comma || peekToken == in_a || peekToken == inout_a ) {

		if ( peekToken == comma ){

			getNextToken();

			if ( peekToken == in_a || peekToken == inout_a ) {

				formalParItem();

			} else {

				error("Parameter needed after comma, in Parameters declaration.");
			}

		} else {
			error("Comma needed between parameter declarations.");
		}
	}
}

void formalParItem() {

	printf("Syntax Debug: Inside formalParItem.\n\n");

	getNextToken();

	if ( token == in_a ) {

		getNextToken();

		if ( token != VARIABLE ) {
			error("No variable declared in 'FormalParItem");
		}

		addEntity(currentLexeme, 4, PASS_BY_VALUE, "0");

	} else if ( token == inout_a ) {

		getNextToken();

		if ( token != VARIABLE ) {
			error("No variable declared in 'FormalParItem");
		}

		addEntity(currentLexeme, 4, PASS_BY_REFERENCE, "0");
	}
}

void sequence() {

	printf("Syntax Debug: Inside sequence.\n\n");

	statement();

	while ( peekToken == semicolon ) {

		getNextToken();

		statement();
	}
}

void brack_or_stat() {

	printf("Syntax Debug: Inside brack_or_stat.\n\n");

	if ( peekToken == curlbrackleft ) {

		brackets_seq();

	} else {

		statement();
	}
}

void brackets_seq() {

	getNextToken();

	if ( token == curlbrackleft ) {

		sequence();

		getNextToken();

		if ( token != curlbrackright ) {

			error("Curly brackets must close after opening.");
		}
	}
}

void statement() {

	printf("Syntax Debug: Inside statement.\n\n");

	// Assignment-Stat:
	if ( peekToken == VARIABLE ) {

		printf("Debug: Inside 'assignment' statement.\n\n");

		// Consume "VARIABLE":
		getNextToken();

		char assignment_target[30];
		strcpy(assignment_target, currentLexeme);
		

		getNextToken();

		if ( token == assign ) {

			char *E_Place = malloc(sizeof(char)*30);
			expression(&E_Place);;

			genquad(":=", E_Place, "_", assignment_target);

		} else {
			error("Assignment symbol ':=' needed after Variable ID");
		}

	// If-Stat:
	} else if ( peekToken == if_a ) {

		printf("Debug: Inside 'if...else' statement.\n\n");

		// Consume "if_a":
		getNextToken();

		getNextToken();

		label_node *B_True;
		label_node *B_False;

		if ( token == parenthleft ) {

			condition(&B_True, &B_False);

			getNextToken();

			if ( token != parenthright ) {
				error("Closing parenthesis needed in 'if' statement condition.");
			}
		}

		// {P1}:
		backpatch(B_True, nextquad());

		brack_or_stat();

		// {P2}:
		label_node *ifList = makelist(nextquad());
		genquad("jump","_","_","_");
		backpatch(B_False, nextquad());

		elsepart();

		// {P3}:
		backpatch(ifList, nextquad());

	// While-Stat:
	} else if ( peekToken == while_a ) {

		printf("Debug: Inside 'while' statement.\n\n");

		// Consume "while_a":
		getNextToken();

		// {P1}:
		int B_Quad = nextquad();

		getNextToken();

		label_node *B_True;
		label_node *B_False;

		if ( token == parenthleft ) {

			condition(&B_True, &B_False);

			getNextToken();

			if ( token != parenthright ) {
				error("Closing parenthesis needed in 'while' condition.");
			}

			// {P2}:
			backpatch(B_True, nextquad());

			brack_or_stat();

			// Turn the (int)B_Quad to a character string:
			char B_Quad_as_String[30];
			sprintf(B_Quad_as_String, "%d", B_Quad);
			genquad("jump", "_", "_", B_Quad_as_String);

			backpatch(B_False, nextquad());
		}

	// Do-While-Stat:
	} else if ( peekToken == do_a ) {

		printf("Debug: Inside 'do...while' statement.\n\n");

		// Consume "do_a":
		getNextToken();

		// {P1}:
		int S_Quad = nextquad();

		brack_or_stat();

		getNextToken();

		if ( token == while_a ) {

			getNextToken();

			label_node *B_True;
			label_node *B_False;

			if ( token == parenthleft ) {

				condition(&B_True, &B_False);

				// {P2}:
				backpatch(B_True, S_Quad);
				backpatch(B_False, nextquad());

				getNextToken();

				if ( token != parenthright ) {

					error("Closing parenthesis needed in 'do-while' statement.");

				}

			} else {

				error("Opening parenthesis needed for 'do-while' condition, after 'while'.");
			}
		}

	// For-Stat:
	} else if ( peekToken == for_a ) {

		printf("Debug: Inside 'for' statement.\n\n");

		// Consume "for_a":
		getNextToken();

		getNextToken();

		label_node *B_True;
		label_node *B_False;

		char *w;

		char *E1_Place;
		char *E2_Place;
		char *E3_Place;

		if ( token == VARIABLE ) {

			getNextToken();

			if ( token == assign ) {
				
				E1_Place = malloc(sizeof(char)*30);
				expression(&E1_Place);

				w = newtemp();
				genquad(":=", E1_Place, "_", w);

				getNextToken();

				if ( token == to_a ) {

					E2_Place = malloc(sizeof(char)*30);
					expression(&E2_Place);
					
					if ( peekToken == step_a ) {

						// Consume "step_a":
						getNextToken();

						E3_Place = malloc(sizeof(char)*30);
						expression(&E3_Place);
					}

				} else {
					error("'For loop' is not syntactically correct: 'to' needed.");
				}

			} else {
				error("'For loop' is not syntactically correct: 'assign' symbol needed.");
			}

		} else {
			error("Iterator variable (called 'id' in grammar) needed in 'for-loop'");
		}

		int For_Quad = nextquad();

		char temp_next_quad_as_string[30];
		sprintf(temp_next_quad_as_string, "%d", nextquad()+20);
		
		genquad("<=", w, E2_Place, temp_next_quad_as_string);

		B_False = makelist(nextquad());
		genquad("jump", "_", "_", "_");

		genquad("+", w, E3_Place, w);

		int statement_to_be_executed = nextquad();

		brack_or_stat();

		char For_Quad_as_String[30];
		sprintf(For_Quad_as_String, "%d", For_Quad);
		genquad("jump", "_", "_", For_Quad_as_String);

		backpatch(B_False, nextquad());

	// Call-Stat:
	} else if ( peekToken == call_a ) {

		printf("Debug: Inside 'call' statement.\n\n");

		// Consume "VARIABLE":
		getNextToken();

		getNextToken();

		if ( token == VARIABLE ) {

			// Hold the function name, before it's consumed:
			char function_name[30];
			strcpy(function_name, currentLexeme);
			

			actualpars();

			genquad("call", function_name, "_", "_");

		} else {
			error("Function ID expected after 'call' declaration.");
		}

	// Return-Stat:
	} else if ( peekToken == return_a ) {

		printf("Debug: Inside 'return' statement.\n\n");

		// Consume "VARIABLE":
		getNextToken();

		getNextToken();

		if ( token == parenthleft ) {

			char *E_Place = malloc(sizeof(char)*30);
			expression(&E_Place);;

			getNextToken();

			if ( token != parenthright ) {
				error("Closing parenthesis needed in 'return' statement");
			}

			genquad("retv", E_Place, "_", "_");

		} else {
			error("Parenthesis needed after 'return' statement.");
		}

	// Print-Stat:
	} else if ( peekToken == print_a ) {

		printf("Debug: Inside 'print' statement.\n\n");

		// Consume "VARIABLE":
		getNextToken();

		getNextToken();

		if ( token == parenthleft ) {

			char *E_Place = malloc(sizeof(char)*30);
			expression(&E_Place);;

			getNextToken();

			if ( token != parenthright ) {
				error("Closing parenthesis needed in 'print' statement");
			}

			genquad("out", E_Place, "_", "_");

		} else {
			error("Parenthesis needed after 'print' statement.");
		}
	}
}


void actualpars() {

	printf("Syntax Debug: Inside actualpars.\n\n");

	getNextToken();

	if ( token == parenthleft ) {

		if ( peekToken == in_a || peekToken == inout_a ) {

			actualparlist();
		}

		getNextToken();

		if ( token != parenthright ) {
			error("Closing parenthesis needed in parameters definition ('actualpars').");
		}

	} else {
		error("Parentheses needed for Parameters definition ('actualpars').");
	}
}

void actualparlist() {

	printf("Syntax Debug: Inside actualparlist.\n\n");

	actualparitem();

	while ( peekToken == comma || peekToken == in_a || peekToken == inout_a ) {

		getNextToken();

		if ( token == in_a || token == inout_a ) {
			error("Parameters need to be seperated with a comma.");
		}

		actualparitem();
	}
}

void actualparitem() {

	printf("Syntax Debug: Inside actualparitem.\n\n");

	getNextToken();

	if ( token == in_a ) {

		char *E_Place = malloc(sizeof(char)*30);

		expression(&E_Place);;

		genquad("par", E_Place, "CV", "_");

	} else if ( token == inout_a ) {

		getNextToken();

		if ( token != VARIABLE ) {
			error("Variable needed after 'inout' of 'call' statement.");
		}

		genquad("par", currentLexeme, "REF", "_");
		

	} else {
		error("Missing 'in' or 'inout' expression after 'call' statement.");
	}
}

void condition(label_node **B_True, label_node **B_False) {

	printf("Syntax Debug: Inside condition.\n\n");

	// {P1}:
	label_node *Q1_True;
	label_node *Q1_False;

	boolterm(&Q1_True, &Q1_False);

	*B_True = Q1_True;
	*B_False = Q1_False;

	while ( peekToken == or_a ) {

		// {P2}:
		backpatch(*B_False, nextquad());

		// Consume the "or_a" token:
		getNextToken();

		label_node *Q2_True;
		label_node *Q2_False;

		boolterm(&Q2_True, &Q2_False);

		// {P3}:
		*B_True = merge(*B_True, Q2_True);
		*B_False = Q2_False;
	}
}

void boolterm(label_node **Q_True, label_node **Q_False) {

	printf("Syntax Debug: Inside boolterm.\n\n");

	label_node *R1_True;
	label_node *R1_False;

	boolFactor(&R1_True, &R1_False);

	// {P1}:
	*Q_True = R1_True;
	*Q_False = R1_False;

	while ( peekToken == and_a ) {

		//Consume the "and_a" token:
		getNextToken();

		// {P2}:
		backpatch(*Q_True, nextquad());

		label_node *R2_True;
		label_node *R2_False;

		boolFactor(&R2_True, &R2_False);

		// {P3}:
		*Q_False = merge(*Q_False, R2_False);
		*Q_True = R2_True;
	}
}

void term(char **T_Place) {

	printf("Syntax Debug: Inside term.\n\n");

	char *F1_Place;
	F1_Place=malloc(sizeof(30));
	factor(&F1_Place);

	while ( peekToken == multipl || peekToken == divide ){

		mul_oper();

		if (token == multipl) {

			char *F2_Place;
			F2_Place = malloc(sizeof(char)*30);

			factor(&F2_Place);

			// {P1}:
			char *w = newtemp();

			addEntity(w, 5, 0, "0");

			genquad("x", F1_Place, F2_Place, w);
			strcpy(F1_Place, w);

		} else if (token == divide) {

			char *F2_Place;
			F2_Place = malloc(sizeof(char)*30);

			factor(&F2_Place);

			// {P1}:
			char *w = newtemp();

			addEntity(w, 5, 0, "0");

			genquad("/", F1_Place, F2_Place, w);
			strcpy(F1_Place, w);
		}
	}

	// {P2}:
	*T_Place = malloc(sizeof(char)*30);
	strcpy(*T_Place, F1_Place);
}

void factor(char **F_Place) {

	printf("Syntax Debug: Inside factor.\n\n");

	getNextToken();

	if ( token == INTEGER ) {

		// Here we have to give the Integer value
		// to the *F_Place value to be returned:

		*F_Place = malloc(sizeof(char)*30);
		strcpy(*F_Place, currentLexeme);

		// TODO: Εδώ δεν θέλει addEntity??

	} else if ( token == parenthleft ) {

		char *E_Place;
		E_Place = malloc(sizeof(char)*30);

		expression(&E_Place);

		getNextToken();

		if ( token != parenthright ) {
			error("Closing parenthesis needed for 'factor' expression.");
		}

		*F_Place = malloc(sizeof(char)*30);
		strcpy(*F_Place, E_Place);

	} else if ( token == VARIABLE ) {

		if ( peekToken == parenthleft ) {

			// Case: ID <IDTAIL>
			char tempLexeme[30];
			strcpy(tempLexeme, currentLexeme);
			
			char *Id_Place = malloc(sizeof(char)*30);

			idtail(&Id_Place);

			genquad("call", tempLexeme, "_", "_");

			*F_Place = malloc(sizeof(char)*30);
			strcpy(*F_Place, Id_Place);

		} else {

			// Case: ID (terminal symbol)

			*F_Place = malloc(sizeof(char)*30);
			strcpy(*F_Place, currentLexeme);
		}
	}
}

void idtail(char **Id_Place) {

	printf("Syntax Debug: Inside idtail.\n\n");

	char temp_lexeme_storage[30];
	strcpy(temp_lexeme_storage, currentLexeme);

	if ( peekToken == parenthleft ) {

		actualpars();

		char *w = newtemp();

		addEntity(w, 5, 0, "0");

		genquad("par", w, "RET", "_");

		*Id_Place = malloc(sizeof(char *)*30);
		strcpy(*Id_Place, w);
	}
}

void relational_oper(char relational_operator[10]) {

	printf("Syntax Debug: Inside relational_oper.\n\n");

	getNextToken();

	if ( token == equals ) {

		strcpy(relational_operator, "==");

	} else if ( token == morethan ) {

		strcpy(relational_operator, ">");

	} else if ( token == lessthan ) {

		strcpy(relational_operator, "<");

	} else if ( token == moreequals ) {

		strcpy(relational_operator, ">=");

	} else if ( token == lessequals ) {

		strcpy(relational_operator, "<=");

	} else if ( token == different ) {

		strcpy(relational_operator, "<>");

	} else {
		error("Relational operator (more-than/less-than etc.) needed.");
	}
}

void add_oper() {

	printf("Syntax Debug: Inside add_oper.\n\n");

	getNextToken();

	if ( token != plus && token != minus ) {
		error("Addition or Subtraction operator needed.");
	}
}

void mul_oper() {

	printf("Syntax Debug: Inside mul_oper.\n\n");

	getNextToken();

	if ( token != multipl && token != divide ) {
		error("Multiplication or Division operator needed.");
	}
}

void optionalSign() {

	printf("Syntax Debug: Inside optionalSign.\n\n");

	if ( peekToken == plus || peekToken == minus ) {
		add_oper();
	}
}

void getNextToken() {

	token = lex();

	// For debugging:
	printf("Got next token: %d (and peeked: %d)\n\n", token, peekToken);
}