CC = gcc -g -Wall -Werror

EXAMPLES = cat_grep ctrl-c regular read-line keyboard-example

all: $(EXAMPLES)

%: %.c
	$(CC) -o $@ $^

clean:
	rm -f $(EXAMPLES)
