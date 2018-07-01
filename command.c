//
// CS252: Shell project
//
// Template file.
// You will need to add more code here to execute the command table.
// You also will probably want to add other files as you add more functionality,
// unless you like having one massive file with thousands of lines of code!
//
// NOTE: You are responsible for fixing any bugs this code may have!
//

#include "command.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include "y.tab.h"

// These global variables are used by shell.y to
// keep track of the current parser state.

command *current_command;
simple_command *current_simple_command;

/*
 * Allocate and initialize a new simple_command.
 * Return a pointer to it.
 */

simple_command *simple_command_create() {
  // Initially allocate space for this many arguments

#define INITIAL_ARUMENT_ARR_SIZE (5)

  simple_command *new_simple_command =
    (simple_command *) malloc(sizeof(simple_command));

  // Create initial space for arguments (may need expansion later)

  new_simple_command->num_available_arguments = INITIAL_ARUMENT_ARR_SIZE;
  new_simple_command->num_arguments = 0;
  new_simple_command->arguments = (char **)
    malloc(new_simple_command->num_available_arguments * sizeof(char *));

  return new_simple_command;
} /* simple_command_create() */

/*
 * Insert an argument into a simple_command.
 * Dynamically expand the argument array if it is too small.
 */

void simple_command_insert_argument(simple_command *s_command, char *argument) {
  if (s_command->num_available_arguments == s_command->num_arguments + 1) {
    // Double the available space

    s_command->num_available_arguments *= 2;
    s_command->arguments =
      (char **) realloc(s_command->arguments,
                        s_command->num_available_arguments * sizeof(char *));
  }

  s_command->arguments[s_command->num_arguments] = argument;

  // NULL argument signals end of arguement list

  s_command->arguments[s_command->num_arguments + 1] = NULL;

  s_command->num_arguments++;
} /* simple_command_insert_argument() */

/*
 * Allocate and initialize a new command. Return a pointer to it.
 */

command *command_create() {
  command *new_command = (command *) malloc(sizeof(command));

  // Create space to point to the first simple command

  new_command->num_available_simple_commands = 1;
  new_command->simple_commands =
    (simple_command **) malloc(new_command->num_available_simple_commands *
                               sizeof(simple_command *));

  new_command->num_simple_commands = 0;
  new_command->out_file = 0;
  new_command->in_file = 0;
  new_command->err_file = 0;
  new_command->is_background = 0;

  new_command->is_append;
  return new_command;
} /* command_create() */

/*
 * Insert a simple_command into the command. If we don't have enough
 * space in simple_commands, dynamically allocate more space.
 */

void command_insert_simple_command(command *command,
                                   simple_command *s_command) {
  if (command->num_available_simple_commands == command->num_simple_commands) {
    // double size of simple command list

    command->num_available_simple_commands *= 2;
    command->simple_commands = (simple_command **)
       realloc(command->simple_commands,
               command->num_available_simple_commands *
               sizeof(simple_command *));
  }

  command->simple_commands[command->num_simple_commands] = s_command;
  command->num_simple_commands++;
} /* command_insert_simple_command() */

/*
 * Completely clear a command, freeing all struct members.
 * After running, the command will be completely ready, as if
 * it was newly created.
 */

void command_clear(command *command) {
  for (int i = 0; i < command->num_simple_commands; i++) {
    for (int j = 0; j < command->simple_commands[i]->num_arguments; j++) {
      free(command->simple_commands[i]->arguments[j]);
      command->simple_commands[i]->arguments[j] = NULL;
    }

    free(command->simple_commands[i]->arguments);
    command->simple_commands[i]->arguments = NULL;

    free(command->simple_commands[i]);
    command->simple_commands[i] = NULL;
  }

  command->num_simple_commands = 0;

  if (command->out_file) {
    free(command->out_file);
    command->out_file = NULL;
  }

  if (command->in_file) {
    free(command->in_file);
    command->in_file = NULL;
  }

  if (command->err_file) {
    free(command->err_file);
    command->err_file = NULL;
  }

  command->is_background = 0;
} /* command_clear */

/*
 * Print the command table for a command.
 * This displays in a human-readable format, all simple_commands
 * in the command, and other metadata in the command including
 * input redirection and background status.
 */

void command_print(command *command) {
  printf("\n\n");
  printf("              COMMAND TABLE                \n");
  printf("\n");
  printf("  #   Simple Commands\n");
  printf("  --- ----------------------------------------------------------\n");

  for (int i = 0; i < command->num_simple_commands; i++) {
    printf("  %-3d ", i);
    for (int j = 0; j < command->simple_commands[i]->num_arguments; j++) {
      printf("\"%s\" \t", command->simple_commands[i]->arguments[j]);
    }
  }

  printf("\n\n");
  printf("  Output       Input        Error        Background\n");
  printf("  ------------ ------------ ------------ ------------\n");
  printf("  %-12s %-12s %-12s %-12s\n",
         command->out_file ? command->out_file : "default",
         command->in_file ? command->in_file : "default",
         command->err_file ? command->err_file : "default",
         command->is_background ? "YES" : "NO");
  printf("\n\n");
} /* command_print() */

/*
 * Execute the command, setting up all input redirection as specified.
 * Current this is basically a no-op. You must make it work.
 */

void command_execute(command *command) {
  // Don't do anything if there are no simple commands

  if (command->num_simple_commands == 0) {
    prompt();
    return;
  }

  // Print contents of Command data structure

  command_print(command);

  // Add execution here
  // For every simple command fork a new process
  // Setup i/o redirection
  // and call exec

  // Clear to prepare for next command

  command_clear(command);

  // Print next prompt

  prompt();
} /* command_execute() */


/*
 * Print the shell prompt
 */

void prompt() {

  if (isatty(0)) {
    printf("myshell>");
  }
  fflush(stdout);
} /* prompt() */

/*
 * Start the shell
 */

int main() {
  // initialize the current_command

  current_command = command_create();

  prompt();

  // run the parser

  yyparse();

  return 0;
} /* main() */
