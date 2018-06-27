//
// read-line: Read a line from the terminal
// Example that shows how to read one line with simple editing
// using raw terminal.
//

#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_LINE (2048)

/*
 * Sets terminal into raw mode.
 * This causes having the characters available
 * immediately instead of waiting for a newline.
 * Also there is no automatic echo.
 */

void tty_raw_mode(void) {
  struct termios tty_attr;

  tcgetattr(0, &tty_attr);

  // Set raw mode

  tty_attr.c_lflag &= (~(ICANON | ECHO));
  tty_attr.c_cc[VTIME] = 0;
  tty_attr.c_cc[VMIN] = 1;

  tcsetattr(0, TCSANOW, &tty_attr);
} /* tty_raw_mode() */

// Buffer where line is stored

int line_length;
char line_buffer[MAX_BUFFER_LINE];

// Simple history array
// This history does not change.
// Yours will have to be updated.

int history_index = 0;
char *history [] = {
  "ls -al | grep x",
  "ps -e",
  "cat read-line-example.c",
  "vi hello.c",
  "make",
  "ls -al | grep xxx | grep yyy"
};
int history_length = sizeof(history) / sizeof(char *);

/*
 * Print usage for the command-line editor
 */

void read_line_print_usage() {

#define USAGE "\n"                                      \
    " ctrl-?       Print usage\n"                       \
    " Backspace    Deletes last character\n"            \
    " up arrow     See last command in the history\n"

  write(1, USAGE, strlen(USAGE));
} /* read_line_print_usage() */

/*
 * Input a line with some basic editing.
 */

char *read_line() {

  // Set terminal in raw mode
  tty_raw_mode();

  line_length = 0;

  // Read one line until enter is typed
  while (1) {

    // Read one character in raw mode from stdin.
    char ch;
    read(0, &ch, 1);

    /* printf("\n%d", ch); */

    if (ch >= 32 && ch < 127) {
      // It is a printable character.

      // Do echo

      write(1, &ch, 1);

      // If max number of character reached, return.

      if (line_length==MAX_BUFFER_LINE-2) {
        break;
      }

      // add char to buffer.

      line_buffer[line_length] = ch;
      line_length++;
    }
    else if (ch == '\n') {
      // <Enter> was typed. Return line

      // Print newline

      write(1, &ch, 1);

      break;
    }
    else if (ch == 31) {
      // ctrl-? (actually ctrl-/)

      read_line_print_usage();

      // reset buffer

      line_buffer[0] = '\0';

      break;
    }
    else if (ch == '\b' || ch == 127) {
      // <backspace> was typed. Remove previous character read.

      // Go back one character

      ch = '\b';
      write(1, &ch, 1);

      // Write a space to erase the last character read

      ch = ' ';
      write(1, &ch, 1);

      // Go back one character

      ch = '\b';
      write(1, &ch, 1);

      // Remove one character from buffer

      line_length--;
    }
    else if (ch == '\e') {
      // Escape sequence. Read two chars more
      //
      // HINT: Use the program "keyboard-example" to
      // see the ascii code for the different chars typed.

      char first_char;
      char second_char;

      read(0, &first_char, 1);
      read(0, &second_char, 1);

      if (first_char == '[' && second_char == 'A') {
        // Up arrow. Print next line in history.

        // Erase old line
        // Print backspaces

        int i = 0;
        for (i = 0; i < line_length; i++) {
          ch = '\b';
          write(1, &ch, 1);
        }

        // Print spaces on top

        for (i = 0; i < line_length; i++) {
          ch = ' ';
          write(1, &ch, 1);
        }

        // Print backspaces

        for (i = 0; i < line_length; i++) {
          ch = '\b';
          write(1, &ch, 1);
        }

        // Copy line from history

        strcpy(line_buffer, history[history_index]);
        line_length = strlen(line_buffer);
        history_index = (history_index + 1) % history_length;

        // echo line

        write(1, line_buffer, line_length);
      }
    }
  }

  // Add eol and null char at the end of string

  line_buffer[line_length] = '\n';
  line_length++;
  line_buffer[line_length] = '\0';

  return line_buffer;
} /* read_line() */

/*
 * Run read_line() in an infinite loop
 */

int main(int argc, char ** argv) {

#define PROMPT "myshell>"

  while (1) {
    char * s;

    // Print prompt

    write(1, PROMPT, strlen(PROMPT));

    // Read one line

    s = read_line();

    // Print line read

    printf("line read: %s\n", s);
  }
} /* main() */
