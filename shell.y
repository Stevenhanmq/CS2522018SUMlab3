//
// CS-252
// shell.y: parser for shell
//
// This parser compiles the following grammar:
//
//   cmd [arg]* [> filename]
//
// you must extend it to understand the complete shell grammar
//


// define all tokens that are used in the lexer here:

%token <string_val> WORD
%token NOTOKEN GREAT NEWLINE GREATGREAT GREATGREATAMP GREATAMP
PIPE AMPERSAND LESS

%union {
  // specify possible types for yylval, for access in shell.l

  char *string_val;

  // int numerical_val;
}

%{

#include <stdio.h>

#include "command.h"

// yyerror() is defined at the bottom of this file

void yyerror(const char * s);

// We must offer a forward declaration of yylex() since it is
// defined by flex and not available until linking.

int yylex();

%}

%%

goal:
  commands
  ;

commands:
  command
  | commands command
  ;

command:
  simple_command
  ;

simple_command:
  pipe_list iomodifier_list NEWLINE {
    printf("   Yacc: Execute command\n");
           command_execute(current_command);
  }
  | NEWLINE
  | error NEWLINE {
    yyerrok;
  }
  ;

command_and_args:
  command_word argument_list {
    command_insert_simple_command(current_command, current_simple_command);
  }
  ;

argument_list:
  argument_list argument
  | // can be empty
  ;

pipe_list:
  pipe_list PIPE command_and_args
  | command_and_args
  ;

argument:
  WORD {
    printf("   Yacc: insert argument \"%s\"\n", $1);

    simple_command_insert_argument(current_simple_command, $1);
  }
  ;

command_word:
  WORD {
    printf("   Yacc: insert command \"%s\"\n", $1);

    current_simple_command = simple_command_create();
    simple_command_insert_argument(current_simple_command, $1);
  }
  ;

iomodifier_opt:
  GREAT WORD {
    //printf("   Yacc: insert output \"%s\"\n", $2);
    if (current_command->out_file == NULL) {
      printf("Ambiguous output redirect\n");
    }
    current_command->out_file = $2;
  }
  |
  GREATGREAT WORD {
    printf("   Yacc: insert output \"%s\"\n", $2);
    current_command->out_file = $2;
  }
  |
  GREATGREATAMP WORD {
    printf("   Yacc: insert output \"%s\"\n", $2);
    current_command->out_file = $2;
  }
  |
  GREATAMP WORD {
    printf("   Yacc: insert output \"%s\"\n", $2);
    current_command->out_file = $2;
  }
  |
  LESS WORD {
    printf("   Yacc: insert output \"%s\"\n", $2);
    current_command->out_file = $2;
  }// can be empty
  ;

iomodifier_list:
  iomodifier_list iomodifier_opt
  |
  ;


%%

/*
 * On parser error, just print the error
 */

void yyerror(const char *message) {
  fprintf(stderr, "%s", message);
} /* yyerror() */
