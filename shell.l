/*
 *
 * CS 252 Summer 2018
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <string.h>
#include <unistd.h>
#include "y.tab.h"

/*char * read_line(){
}
int mygetc(FILE * f) {
static char *p;
char ch;
if (!isatty(0)) {
  // stdin is not a tty. Call real getc
  return getc(f);
}
// stdin is a tty. Call our read_line.
if (p==NULL || *p == 0) {
  char * s = read_line();
  p = s;
}
ch = *p;
p++;
return ch;
}
#undef getc
#define getc(f) mygetc(f)
*/
%}

%%

\n {
  return NEWLINE;
}

[ \t] {
  // Discard spaces and tabs

}

">" {
  return GREAT;
}

">>" {
  return GREATGREAT;
}

"&" {
  return AMPERSAND;
}

\"[^\t\n\"]*\" {
//  printf("%s getinherre?\n", yytext);
  char * p = yytext;
  char * w = (char*)malloc(200);
  char * tmp = w;
  while (*p){
    if(*p == '\"'){
      p++;
      while(*p != '\"'){
        *tmp = *p;
        p++;
        tmp++;
      }
    }
    p++;
  } 
  *tmp = '\0';
  yytext = strdup(w);
  yylval.string_val = strdup(yytext);
  return WORD;
}

"|" {
  return PIPE;
}

"<" {
  return LESS;
}

">&" {
  return GREATAMP;
}

">>&" {
  return GREATGREATAMP;
}

"2>" {
  return TWOGREAT;
}

[^ \t\n][^ \t\n]* {
  // Assume that file names have only alpha chars

  yylval.string_val = strdup(yytext);
  return WORD;
}


%%
