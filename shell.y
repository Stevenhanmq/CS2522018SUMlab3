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
PIPE AMPERSAND LESS TWOGREAT

%union {
  // specify possible types for yylval, for access in shell.l

  char *string_val;

  // int numerical_val;
}

%{

#include <stdio.h>

#include "command.h"
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
// yyerror() is defined at the bottom of this file

void yyerror(const char * s);
void expand_wildcards(char * prefix, char * suffix);
int mycompare(const void *s1, const void *s2);
// We must offer a forward declaration of yylex() since it is
// defined by flex and not available until linking.
char ** filelist;
int num_entries;
int max_entries = 100;
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
  pipe_list iomodifier_list background_opt NEWLINE {
    //printf("   Yacc: Execute command\n");
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
    //printf("   Yacc: insert argument \"%s\"\n", $1);
    if(!(strchr($1,'*')) && !(strchr($1,'?'))) {
      simple_command_insert_argument(current_simple_command, strdup($1));
    }
    else {
      if(filelist == NULL) {
        filelist = (char**)malloc(max_entries*sizeof(char*));
      }
      char * nopref = "";
      expand_wildcards(nopref, strdup($1));
      if (num_entries == 0) {
         simple_command_insert_argument(current_simple_command, strdup($1));
      }
      else {
        qsort(filelist, num_entries, sizeof(char *), mycompare);
        for(int i = 0; i < num_entries; i++) {
          simple_command_insert_argument(current_simple_command,
				       strdup(filelist[i]));
        }
        if(filelist != NULL){
	  free(filelist);
        }
        num_entries = 0;
        filelist = NULL;
      }
    }
    //wildcard_test(current_simple_command, strdup($1));
  }
  ;

command_word:
  WORD {
    //printf("   Yacc: insert command \"%s\"\n", $1);

    current_simple_command = simple_command_create();
    simple_command_insert_argument(current_simple_command, strdup($1));
  }
  ;

iomodifier_opt:
  GREAT WORD {
    //printf("   Yacc: insert output \"%s\"\n", $2);
    if (current_command->out_file) {
      printf("Ambiguous output redirect.\n");
    }
    current_command->out_file = strdup($2);
  }
  |
  GREATGREAT WORD {
    //printf("   Yacc: insert output \"%s\"\n", $2);
    current_command->out_file = strdup($2);
    current_command->is_append = 1;
  }
  |
  GREATGREATAMP WORD {
    //    printf("   Yacc: insert output \"%s\"\n", $2);
    //current_command->out_file = strdup($2);
    current_command->err_file = strdup($2);
    current_command->is_append = 1;
  }
  |
  TWOGREAT WORD {
    current_command->out_file = strdup($2);
    current_command->err_file = strdup($2);
  }
  |
  GREATAMP WORD {
    //printf("   Yacc: insert output \"%s\"\n", $2);
    //current_command->out_file = strdup($2);
    current_command->err_file = strdup($2);
  }
  |
  LESS WORD {
    //printf("   Yacc: insert output \"%s\"\n", $2);
    current_command->in_file = strdup($2);
  }// can be empty
  ;

iomodifier_list:
  iomodifier_list iomodifier_opt
  |
  ;

background_opt:
  AMPERSAND {
    current_command->is_background = 1;
  }
  |
  ;

%%







/*
 * On parser error, just print the error
 */
#define MAXFILENAME 1024
void expand_wildcards (char * prefix, char * suffix) {
  if(strlen(suffix) == 0){
    if(num_entries == max_entries){
      max_entries += 100;
      filelist = (char**)realloc(filelist, max_entries*sizeof(char*));
    }
    filelist[num_entries] = strdup(prefix);
    num_entries +=1;
    return;
  }  
}

int mycompare (const void *s1, const void *s2) {
  char * string1 = strdup(s1);
  char * string2 = strdup(s2);
  //  printf("%s,%s\n",string1, string2);
  return (strcmp(string1, string2));
}

void yyerror(const char *message) {
  fprintf(stderr, "%s", message);
} /* yyerror() */
