//
// ctrl-c: Example of how to ignore ctrl-c
//

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

/*
 * Handle sigint. In our case, just print a message.
 */

void disp(int sig) {
  fprintf(stderr, "\n      Ouch!\n");
} /* disp() */

/*
 * Set up a simple prompt that will ignore everything you type
 * except for "exit", at which point it will exit.
 * It will also ignore ctrl-c, and just print a message instead of
 * quitting.
 */

int main() {

#define BUFFER_SIZE (20)

  printf("Type ctrl-c or \"exit\"\n");


  // set up signal handling

  struct sigaction sa;

  // use disp() as the handler

  sa.sa_handler = disp;

  // clear mask (that is, continue to recieve all other signals
  // while in the handler)

  sigemptyset(&sa.sa_mask);

  // use default flags

  sa.sa_flags = 0;

  // register signal handler

  if (sigaction(SIGINT, &sa, NULL)) {
    perror("sigaction");
    exit(2);
  }


  for ( ; ; ) {
    char line[BUFFER_SIZE];
    printf("prompt> ");
    fflush(stdout);
    fgets(line, BUFFER_SIZE, stdin);

    if (!strcmp(line, "exit\n")) {
      printf("Bye!\n");
      exit(1);
    }
  }

  return 0;
} /* main() */
