//
// regular: Check a string against a regular expression
//

#include <sys/types.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

#define USAGE ""                                                        \
  "Usage:\n"                                                            \
  "      regular regular-expresion string\n"                            \
  "\n"                                                                  \
  "      Tells if \"string\" matches \"regular-expresion\".\n"          \
  "\n"                                                                  \
  "      '^' and '$' characters are added at the beginning and\n"       \
  "      end of \"regular-expresion\" to force match of the entire\n"   \
  "      \"string\"\n"                                                  \
  "\n"                                                                  \
  "      To know more about regular expresions type \"man ed\"\n"       \
  "Try:\n"                                                              \
  "      bash> ./regular aaa aaa\n"                                     \
  "      bash> ./regular \"a*\" aaaa\n"                                 \
  "      bash> ./regular \"a*\" aaa\n"                                  \
  "      bash> ./regular \"a*\" aaaf\n"                                 \
  "      bash> ./regular \"a.*\" akjhkljh \n"                           \
  "      bash> ./regular \"a.*\" jkjhkj\n"                              \
  "      bash> ./regular \"a.*\" aaalklkjlk\n"                          \
  "      bash> ./regular \".*\\..*\" kljhkljhlj.lkjhlkj\n"              \
  "      bash> ./regular \".*\\..*\" kljhkljhlj\n\n"

/*
 * Compile the regular expression passed in the first argument,
 * and see if it matches the second argument
 */

int main(int argc, char **argv) {

#define NUM_ARGS (3)
#define REGEX_MAX_SIZE (1024)

  if (argc < NUM_ARGS) {
    fprintf(stderr, "%s", USAGE);
    exit(-1);
  }

  const char *regular_exp = argv[1];
  const char *string_to_match = argv[2];

  // Add ^ and $ at the beginning and end of regular expression
  // to force match of the entire string. See ed(1).

  char regexp_complete[REGEX_MAX_SIZE];
  snprintf(regexp_complete, REGEX_MAX_SIZE, "^%s$", regular_exp);

  // Compile the regular expression, and check results.
  // We won't be needing match position, so use REG_NOSUB.

  regex_t re;
  int result = regcomp(&re, regexp_complete, REG_EXTENDED | REG_NOSUB);
  if (result != 0) {
    fprintf(stderr, "%s: Bad regular expresion \"%s\"\n",
            argv[0], regexp_complete);
    exit(-1);
  }

  // apply regular expression

  regmatch_t match;
  result = regexec(&re, string_to_match, 1, &match, 0);

  const char *match_result = "MATCHES";
  if (result != 0) {
    match_result = "DOES NOT MATCH";
  }

  fprintf(stderr, "\t\"%s\" %s \"%s\"\n", match_result,
          regexp_complete, string_to_match);

  regfree(&re);

  return 0;
} /* main() */
