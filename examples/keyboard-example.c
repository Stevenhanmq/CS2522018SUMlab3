//
// keyboard-example: print ascii values for input
// see ascii(7) for more info
//

#include <termios.h>
#include <stdio.h>
#include <unistd.h>

/*
 * In an infinite loop, read chars and print their ASCII values
 */

int main(int argc, char ** argv) {
  // Set raw mode

  struct termios tty_attr;
  tcgetattr(0, &tty_attr);
  tty_attr.c_lflag &= (~(ICANON | ECHO));
  tty_attr.c_cc[VTIME] = 0;
  tty_attr.c_cc[VMIN] = 1;
  tcsetattr(0, TCSANOW, &tty_attr);

  while (1) {
    // read and print value

    char ch;
    read(0, &ch, 1);
    printf("%d\n", ch);
  }
} /* main() */
