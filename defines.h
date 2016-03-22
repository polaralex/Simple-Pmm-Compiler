// States:
#define STATE_WHITESPACE 60
#define STATE_VARIABLE 61
#define STATE_NUMBER 62

#define STATE_ERROR 69
#define STATE_EOF 70

// For punctuation character identification:

#define SIMPLE_CHARACTER 101
#define POSSIBLE_COMPLEX_CHAR 102
#define REQUIRES_CLOSURE 103
#define IS_CLOSURE_CHARACTER 104

// Sensitive words:
#define and_a 1
#define var_a 2
#define do_a 3
#define else_a 4
#define procedure_a 5
#define function_a 6
#define print_a 7
#define if_a 8
#define in_a 9
#define inout_a 10
#define not_a 11
#define for_a 12
#define to_a 13
#define step_a 14
#define program_a 15
#define or_a 16
#define return_a 17
#define while_a 18
#define call_a 19

// Temporary defines (To-be changed):
#define begin_a 201
#define end_a 202
#define then_a 203

#define NOT_SENSITIVE -50

// Operators:
#define plus 20
#define minus 21
#define multipl 22
#define divide 23
#define lessthan 24
#define morethan 25
#define equals 26
#define lessequals 27
#define moreequals 28
#define different 29
#define assign 30
#define semicolon 31
#define comma 32
#define curlbrackleft 33
#define curlbrackright 34
#define parenthleft 35
#define parenthright 36
#define bracketleft 37
#define bracketright 38
#define commentbegin 39
#define commentend 40

// Other:
#define VARIABLE 41
#define INTEGER 42
#define eof_a 43

#define BIG_CHARACTER_ARRAY_LENGTH 1024
#define DEFAULT_WORD_ARRAY_SIZE 128

// Error:
#define VARIABLE_ERROR -1

// Intercode Helper Defines:
#define IT_IS_MAIN_BLOCK true
#define IT_IS_NOT_MAIN_BLOCK false