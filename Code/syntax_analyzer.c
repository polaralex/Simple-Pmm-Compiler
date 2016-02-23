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

// Helper Functions:
void getNextToken();

/* ------------------------------------------------- */
/*      Beginning of Syntax Analyzer Functions:      */
/* ------------------------------------------------- */

void program() {

	printf("Syntax Debug: Inside program. ");
	printf("Token is %d (peeked is %d)\n", token, peekToken);

	if( token == program_a ){

		getNextToken();

		if (token == VARIABLE ) {

			block();

		} else {

			error("Program name expected.");

		}

	} else {

		error("The keyword 'program' was expected.");

	}
}

void block() {

	printf("Syntax Debug: Inside Block.\n");

	getNextToken();

	if (token == curlbrackleft) {

		getNextToken();

		// Todo: Maybe this needs a 'while-loop' - or maybe it's needed
		// inside the 'declarations' function:
		declarations();

		subprograms();
		sequence();

		getNextToken();

		if( token == curlbrackright ) {

			getNextToken();

		} else {

			error("The keyword 'begin' was expected.");

		}
	}

}

void declarations() {

	printf("Syntax Debug: Inside declarations.\n");

	getNextToken();

	if ( token == var_a ) {

		varlist();

		getNextToken();

		if ( token == semicolon ){

			getNextToken();

		} else {

			error("Semicolon required at the end of variable declarations.");
		
		}
	}

}

void varlist() {

	printf("Syntax Debug: Inside varlist.\n");

	getNextToken();

	if (token == var_a) {

		getNextToken();

		while ( token == comma || token == var_a ) {

			if ( token == comma ) {

				getNextToken();

				if ( token == var_a ){

					getNextToken();

				} else {

					error("Excess commas found in variable declarations.");
				
				}

			} else {

				error("Comma needed between variable declarations.");

			}
		}
	}
}

void if_stat() {

	printf("Syntax Debug: Inside if_stat.\n");

	getNextToken();

	if ( token == if_a ){

		getNextToken();

		if ( token == parenthleft ){

			condition();

			if ( token == parenthright ){

				getNextToken();

			} else {

				error("'If statement' parenthesis not closed.");
			
			}

		}

		brack_or_stat();
		elsepart();

	}

}

void elsepart(){

	printf("Syntax Debug: Inside elsepart.\n");

	getNextToken();

	if (token == else_a) {

		brack_or_stat();

	}

}

void boolFactor() {

	printf("Syntax Debug: Inside boolFactor.\n");

	if (token == not_a) {

		getNextToken();
		condition();

	} else if ( token == parenthleft ) {

		getNextToken();
		condition();

		if ( token == parenthright ) {

			getNextToken();

		} else {

			error("Right Bracket expected.");

		}

	} else {

		expression();
		relational_oper();
		expression();

	}

}

void expression() {

	printf("Syntax Debug: Inside expression.\n");

	optionalSign();
	term();

	getNextToken();

	while (token == plus || token == minus) {

		add_oper();
		term();

		getNextToken();

	}
}

void subprograms() {

	printf("Syntax Debug: Inside Subprograms.\n");

	// To-do: How to do this loop?
	func();

}


void func() {

	printf("Syntax Debug: Inside func.\n");

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
	}
}

void funcBody() {

	printf("Syntax Debug: Inside funcBody.\n");

	formalPars();
	block();

}

void formalPars() {

	printf("Syntax Debug: Inside formalPars.\n");

	getNextToken();

	if ( token == parenthleft ) {

		getNextToken();

		if ( token != parenthright ) {

			formalParList();

		}
	}

	getNextToken();

	if ( token != parenthright ) {

		error("No closing parenthesis found.");

	}
}

void formalParList() {

	printf("Syntax Debug: Inside formalParList.\n");

	formalParItem();

	getNextToken();

	// To-do: This 'while-loop' may need some fixing.
	// Let's see about it...
	// Also: Maybe I can do this recursively (Haskell-style)
	// by calling formalParList itself
	while ( peekToken == comma ) {

		formalParItem();
		getNextToken();

		// Make sure that the next function does not call
		// getNextToken in the first time (since that's already
		// been done here)

	}
}

void formalParItem() {

	printf("Syntax Debug: Inside formalParItem.\n");

	// We don't call getNextToken() here, because we have the next
	// token ready from formalParList()

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

	printf("Syntax Debug: Inside sequence.\n");

	statement();

	getNextToken();

	while ( token == semicolon ) {

			statement();
			getNextToken();

	}

}

void brack_or_stat() {

	printf("Syntax Debug: Inside brack_or_stat.\n");

}

void statement() {

	printf("Syntax Debug: Inside statement.\n");

	getNextToken();

	// Assignment-Stat:
	if ( token == VARIABLE ) {

		getNextToken();

		if ( token == assign ) {

			expression();

		}

	// If-Stat:
	} else if ( token == if_a ) {

		getNextToken();

		if ( token == parenthleft ) {

			condition();
			getNextToken();

			if ( token != parenthright ) {

				error("Closing parenthesis needed in 'if' statement condition.");
			
			}
		}

		brack_or_stat();
		elsepart();

	// While-Stat:
	} else if ( token == while_a ) {

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
	} else if ( token == do_a ) {

		brack_or_stat();
		getNextToken();

		if ( token == while_a ) {

			getNextToken();

			if ( token == parenthleft ) {

				condition();
				getNextToken();

				if ( token !=  parenthright ) {

					error("Closing parenthesis needed in 'do-while' statement.");

				}

			} else {

				error("Opening parenthesis needed for 'do-while' condition.");

			}

		}

	// For-Stat:
	} else if ( token == for_a ) {

		getNextToken();

		if ( token == VARIABLE ) {

			getNextToken();

			if ( token == assign ) {

				expression();
				getNextToken();

				if ( token == to_a ) {

					expression();
					getNextToken();

					if ( token == step_a ) {
						expression();
					}

				} else {
					error("'For loop' is not syntactically correct.");
				}

			} else {
				error("'For loop' is not syntactically correct.");
			}

		} else {
			error("Iterator variable needed in 'for-loop'");
		}

	// Call-Stat:
	} else if ( token == call_a ) {

		getNextToken();

		if ( token == VARIABLE ) {

			actualpars();

		}

	// Return-Stat:
	} else if ( token == return_a ) {

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
	} else if ( token == print_a ) {

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

	printf("Syntax Debug: Inside actualpars.\n");

	getNextToken();

	if ( token == parenthleft ) {

		actualparlist();
		getNextToken();

		if ( token != parenthright ) {

			error("Closing parenthesis needed in 'actualpars");

		}
	}
}

void actualparlist() {

	printf("Syntax Debug: Inside actualparlist.\n");

	actualparitem();

	getNextToken();

	while ( token == comma ) {

			actualparitem();
			getNextToken();

	}
}

void actualparitem() {

	printf("Syntax Debug: Inside actualparitem.\n");

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

void condition() {

	printf("Syntax Debug: Inside condition.\n");

	boolterm();

	getNextToken();

	while ( token == or_a ) {

		boolterm();
		getNextToken();

	}
}

void boolterm() {

	printf("Syntax Debug: Inside boolterm.\n");

	boolFactor();

	getNextToken();

	// To-do: These 'whiles' may have a problem with the fact
	// that they get the next token before the next function
	// demands the right one
	while ( token == and_a ) {

			boolFactor();
			getNextToken();

	}
}

void term() {

	printf("Syntax Debug: Inside term.\n");

	factor();
	getNextToken();

	while ( token == multipl || token == divide ){

		mul_oper();
		factor();

	}
}

void factor() {

	printf("Syntax Debug: Inside factor.\n");

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

		idtail();

	}

}

void idtail() {

	printf("Syntax Debug: Inside idtail.\n");

	getNextToken();

	// To-do: This will not work because we have already used
	// the valid 'left parenthesis' token. Must find a solution:
	if ( token == parenthleft ) {
		actualpars();
	}

}

void relational_oper() {

	printf("Syntax Debug: Inside relational_oper.\n");

	getNextToken();

	if ( token != equals && token != morethan && token != lessthan && token != moreequals && token != lessequals && token != different){
		error("Relational operator (more-than/less-than etc.) needed.");
	}

}

void add_oper() {

	printf("Syntax Debug: Inside add_oper.\n");

	if ( token != plus && token != minus ) {
		error("Addition or Subtraction operator needed.");
	}

}

void mul_oper() {

	printf("Syntax Debug: Inside mul_oper.\n");

	//getNextToken();

	if ( token != multipl && token != divide ) {

		error("Multiplication or Division operator needed.");

	}

}

void optionalSign() {

	printf("Syntax Debug: Inside optionalSign.\n");

	// To-do.

}

void getNextToken() {

	token = lex();

	// For debugging:
	printf("Got next token: %d (and peeked: %d)\n\n", token, peekToken);

}