#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "defines.h"

// P-- Grammar:
void program();
void block();
void declarations();
void expression();
void brack_or_stat();
void formalParItem();
void statement();
void actualpars();
void actualparlist();
void actualparitem();
void boolterm();
void boolFactor();
void factor();
void mul_oper();
void if_stat();
void idtail();
void elsepart();
void varlist();
void relational_oper();
void optionalSign();
void term();
void add_oper();
void subprograms();
void sequence();
void condition();
void funcBody();
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

			block();

		} else {

			error("Program name expected.");

		}

	} else {

		error("The keyword 'program' was expected.");

	}
}

void block() {

	printf("Syntax Debug: Inside Block.\n\n");

	getNextToken();

	if (token == curlbrackleft) {

		declarations();
		subprograms();
		sequence();

		getNextToken();

		if( token != curlbrackright ) {
			error("The keyword 'begin' was expected.");
		}
	}
}

void declarations() {

	printf("Syntax Debug: Inside declarations.\n\n");

	if (peekToken == var_a) {

		// Consume 'var':
		getNextToken();

		varlist();
	}
}

void varlist() {

	printf("Syntax Debug: Inside varlist.\n\n");

	if (peekToken == VARIABLE) {

		// Consume the 'var':
		getNextToken();

		while ( peekToken == comma || peekToken == VARIABLE ) {

			if ( peekToken == comma ) {

				// Consume the 'comma':
				getNextToken();

				if ( peekToken != VARIABLE ) {

					error("Excess commas found in variable declarations.");

				} else {

					getNextToken();

				}

			} else {

				error("Comma needed between variable declarations.");
			}
		}
	} else {

		error("Variable declaration(s) needed after 'var' keyword.");
	}
}

void if_stat() {

	printf("Syntax Debug: Inside if_stat.\n\n");

	getNextToken();

	if ( token == if_a ){

		getNextToken();

		if ( token == parenthleft ){

			condition();

			getNextToken();

			if ( token != parenthright ){

				error("'If statement' parenthesis not closed.");

			}
		}

		brack_or_stat();
		elsepart();
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

void boolFactor(label_node **R_True, label_node **R2_False) {

	printf("Syntax Debug: Inside boolFactor.\n\n");

	getNextToken();

	// Case 1:
	if (peekToken == not_a) {

		// Consume 'not' token:
		getNextToken();

		getNextToken();

		if (token == bracketleft) {

			label_node *B_True;
			label_node *B_False;

			condition(&B_True, &B_False);

			//TODO: Look at this again!

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

		// Consume 'left bracketToken'
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

		// TODO: This after I make the expression() work

		expression();
		relational_oper();
		expression();

	}

}

void expression() {

	printf("Syntax Debug: Inside expression.\n\n");

	optionalSign();

	char *T1_Place;
	T1_Place = malloc(sizeof(30));
	term(&T1_Place);

	while (peekToken == plus || peekToken == minus) {

		add_oper();

		char *T2_Place;
		T2_Place = malloc(sizeof(30));

		term(&T2_Place);

		// {P1}:
		char *w = newtemp();
		genquad("+", T1_Place, T2_Place, w);
		T1_Place = w;

	}

	// {P2}:
	char *E_place = malloc(sizeof(30));
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

	if (token == procedure_a) {

		getNextToken();

		if (token == VARIABLE) {

			funcBody();

		}

	} else if (token == function_a) {

		getNextToken();

		if (token == VARIABLE) {

			funcBody();

		}

	} else {

		error("Function declaration (procedure or function) needed.");
	}
}

void funcBody() {

	printf("Syntax Debug: Inside funcBody.\n\n");

	formalPars();
	block();

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

	} else if ( token == inout_a ) {

		getNextToken();

		if ( token != VARIABLE ) {

			error("No variable declared in 'FormalParItem");
		
		}
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

		// Consume "VARIABLE":
		getNextToken();

		getNextToken();

		if ( token == assign ) {

			expression();

		} else {
			error("Assignment symbol needed after Variable ID");
		}

	// If-Stat:
	} else if ( peekToken == if_a ) {

		// Consume "if_a":
		getNextToken();

		getNextToken();

		if ( token == parenthleft ) {

			label_node *ifTrue;
			label_node *ifFalse;

			condition(&ifTrue, &ifFalse);

			// {P1}
			backpatch(ifTrue, getnextquad());

			getNextToken();

			if ( token != parenthright ) {

				error("Closing parenthesis needed in 'if' statement condition.");
			}
		}

		brack_or_stat();

		// {P2}
		label_node *ifList = makelist(getnextquad());
		genquad("jump","_","_","_");
		backpatch(ifFalse, nextquad);

		elsepart();

		// {P3}
		backpatch(ifList, getnextquad());

	// While-Stat:
	} else if ( peekToken == while_a ) {

		// Consume "while_a":
		getNextToken();

		getNextToken();

		if ( token == parenthleft ) {

			condition();

			getNextToken();

			if ( token != parenthright ) {

				error("Closing parenthesis needed in 'while' condition.");

			}

			brack_or_stat();

		}

	// Do-While-Stat:
	} else if ( peekToken == do_a ) {

		// Consume "do_a":
		getNextToken();

		brack_or_stat();

		getNextToken();

		if ( token == while_a ) {

			getNextToken();

			if ( token == parenthleft ) {

				condition();

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

		// Consume "for_a":
		getNextToken();

		getNextToken();

		if ( token == VARIABLE ) {

			getNextToken();

			if ( token == assign ) {

				expression();

				getNextToken();

				if ( token == to_a ) {

					expression();
					
					if ( peekToken == step_a ) {

						// Consume "step_a":
						getNextToken();

						expression();

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

	// Call-Stat:
	} else if ( peekToken == call_a ) {

		// Consume "VARIABLE":
		getNextToken();

		getNextToken();

		if ( token == VARIABLE ) {

			actualpars();

		}

	// Return-Stat:
	} else if ( peekToken == return_a ) {

		// Consume "VARIABLE":
		getNextToken();

		getNextToken();

		if ( token == parenthleft ) {

			expression();

			getNextToken();

			if ( token != parenthright ) {

				error("Closing parenthesis needed in 'return' statement");

			}

		} else {

			error("Parenthesis needed after 'return' statement.");

		}

	// Print-Stat:
	} else if ( peekToken == print_a ) {

		// Consume "VARIABLE":
		getNextToken();

		getNextToken();

		if ( token == parenthleft ) {

			expression();

			getNextToken();

			if ( token != parenthright ) {

				error("Closing parenthesis needed in 'print' statement");

			}

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

		expression();

	} else if ( token == inout_a ) {

		getNextToken();

		if ( token != VARIABLE ) {

			error("Variable needed after 'inout' of 'call' statement.");

		}

	} else {

		error("Missing 'in' or 'inout' expression after 'call' statement.");

	}
}

// TODO : Condition needs double-pointer (???) parameters
void condition(label_node *B_True, label_node *B_False) {

	printf("Syntax Debug: Inside condition.\n\n");

	// {P1}:
	label_node *Q1_True;
	label_node *Q1_False;

	boolterm(&Q1_True, &Q2_False);

	*B_True = Q1_True;
	*B_False = Q1_False;

	while ( peekToken == or_a ) {

		// {P2}:
		backpatch(*ConditionFalse, getnextquad());

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
	label_node *R2_False;

	boolFactor(&R1_True, &R2_False);

	// {P1}:
	*Q_True = R1_True;
	*Q_False = R1_False;

	while ( peekToken == and_a ) {

		//Consume the "and_a" token:
		getNextToken();

		// {P2}:
		backpatch(*Q_True, getnextquad());

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
	F1_Place = malloc(sizeof(30));

	factor(&F1_Place);

	while ( peekToken == multipl || peekToken == divide ){

		mul_oper();

		char *F2_Place;
		F2_Place = malloc(sizeof(30));

		factor(&F2_Place);

		// {P1}:
		char *w = newtemp();
		genquad("x", F1_Place, F2_Place, w);
		F1_Place = w;
	}

	// {P2}:
	*T_Place = malloc(sizeof(30));
	strcpy(*T_Place, F1_Place);
}

void factor(char **F_Place) {

	printf("Syntax Debug: Inside factor.\n\n");

	getNextToken();

	if ( token == INTEGER ) {

		// do nothing

	} else if ( token == parenthleft ) {

		expression();

		getNextToken();

		if ( token != parenthright ) {
			error("Closing parenthesis needed for 'factor' expression.");
		}

	} else if ( token == VARIABLE ) {

		char *Id_Place;
		Id_Place = malloc(sizeof(30));

		idtail(Id_Place);

	}
}

void idtail(char *Id_Place) {

	printf("Syntax Debug: Inside idtail.\n\n");

	if ( peekToken == parenthleft ) {

		actualpars();

		char *w = newtemp();
		genquad("par", w, "RET", "_");
		genquad("call", Id_Place, "_", "_");
		// TODO: How to return this??? And go upstairs...

	}
}

void relational_oper() {

	printf("Syntax Debug: Inside relational_oper.\n\n");

	getNextToken();

	if ( token != equals && token != morethan && token != lessthan && token != moreequals && token != lessequals && token != different){
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