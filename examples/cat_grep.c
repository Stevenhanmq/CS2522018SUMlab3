//
// cat_grep: Run `cat $1 | grep $2 > $3`
// This example program takes a file as argument, and then passes
// it through grep to search for the second argument. The output
// goes into the file specified by the third argument
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// CAT and GREP specify the commands used to invoke the cat and grep
// programs. For instance, you could define CAT as "head", and then
// cat_grep will only search in the first 10 lines.

#define USAGE ""                                            \
  "Usage:\n"                                                \
  "    cat_grep file-name word outfile\n"                   \
  "\n"                                                      \
  "    It does something similar to the shell command:\n"   \
  "        csh> cat file | grep word > outfile\n"           \
  "\n"                                                      \
  "Example:\n"                                              \
  "    cat_grep command.cc Command out\n"                   \
  "    cat out\n\n"
#define CAT "cat"
#define GREP "grep"

/*
 * Start cat, passing as an argument the file specified by
 * the first argument to cat_grep, and redirecting its output into a pipe.
 * Then start grep, redirecting its input from the pipe, and its
 * output to the file specified by the third arument. The argument
 * to grep is the second argument to cat_grep.
 */

int main(int argc, char **argv, char **envp) {
  if (argc < 4) {
    fprintf(stderr, "%s", USAGE);
    exit(1);
  }

  // Save default input, output, and error because we will
  // change them during redirection and we will need to restore them
  // at the end.

  int default_in = dup(0);
  if (default_in == -1) {
    perror("cat_grep: input dup\n");
    exit(2);
  }
  int default_out = dup(1);
  if (default_out == -1) {
    perror("cat_grep: output dup\n");
    exit(2);
  }
  int default_err = dup(2);
  if (default_err == -1) {
    perror("cat_grep: error dup\n");
    exit(2);
  }

  //////////////////  cat //////////////////////////

  // Input:    default_in
  // Output:   pipe
  // Error:    default_err

  // Create new pipe

  int fd_pipe[2];
  if (pipe(fd_pipe) == -1) {
    perror("cat_grep: pipe\n");
    exit(2);
  }

  // Redirect input
  // Just use normal stdin.
  // This allows us to specify "-" as the arument for cat
  // and then cat will read from stdin (if you want)

  if (dup2(default_in, 0) == -1) {
    perror("cat_grep: cat input dup2\n");
    exit(2);
  }

  // Redirect output to pipe
  // The other end of the pipe (fd_pipe[0]) will be the
  // input to grep

  if (dup2(fd_pipe[1], 1) == -1) {
    perror("cat_grep: cat output dup2\n");
    exit(2);
  };

  // Redirect err
  // Just use good ol' stderr

  if (dup2(default_err, 2) == -1) {
    perror("cat_grep: cat error dup2\n");
    exit(2);
  };

  // Create new process for "cat"

  int pid = fork();
  if (pid == -1) {
    perror("cat_grep: fork\n");
    exit(2);
  }

  if (pid == 0) {
    // Child

    // close file descriptors that are not needed

    close(fd_pipe[0]);
    close(fd_pipe[1]);
    close(default_in);
    close(default_out);
    close(default_err);

    // Start cat, which will take over the child process
    // You can use execvp() instead if the arguments are stored in an array

    execlp(CAT, CAT, argv[1], NULL);

    // exec() is not supposed to return, something went wrong

    perror("cat_grep: exec cat");
    exit(2);
  }

  //////////////////  grep //////////////////////////

  // Input:    pipe
  // Output:   outfile
  // Error:    default_err

  // Redirect input from pipe

  if (dup2(fd_pipe[0], 0) == -1) {
    perror("cat_grep: grep input dup2\n");
    exit(2);
  }

  dup2(fd_pipe[0], 0);

  // Redirect output to outfile

  int outfd = creat(argv[3], 0666);

  if (outfd < 0) {
    perror("cat_grep: creat outfile");
    exit(2);
  }

  if (dup2(outfd, 1) == -1) {
    perror("cat_grep: grep output dup2\n");
    exit(2);
  };

  close(outfd);

  // Redirect err, just use stderr

  if (dup2(default_err, 2) == -1) {
    perror("cat_grep: grep error dup2\n");
    exit(2);
  };

  pid = fork();

  if (pid == -1) {
    perror("cat_grep: fork");
    exit(2);
  }

  if (pid == 0) {
    // Child

    // close file descriptors that are not needed

    close(fd_pipe[0]);
    close(fd_pipe[1]);
    close(default_in);
    close(default_out);
    close(default_err);

    execlp(GREP, GREP, argv[2], NULL);

    // exec() is not suppose to return, something went wrong

    perror("cat_grep: exec grep");
    exit(2);

  }

  // Restore input, output, and error to their initial state

  if (dup2(default_in, 0) == -1) {
    perror("cat_grep: restore input dup2\n");
    exit(2);
  }
  if (dup2(default_out, 1) == -1) {
    perror("cat_grep: restoreoutput dup2\n");
    exit(2);
  };
  if (dup2(default_err, 2) == -1) {
    perror("cat_grep: restoreerror dup2\n");
    exit(2);
  };

  // Close file descriptors that are not needed

  close(fd_pipe[0]);
  close(fd_pipe[1]);
  close(default_in);
  close(default_out);
  close(default_err);

  // Wait for last process in the pipe line

  waitpid(pid, 0, 0);

  exit(2);
} /* main() */
