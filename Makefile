NAME = bin-srch-tree
OBJS = main.o tree.o

STRIP = strip

CFLAGS = -Wall -Werror

.PHONY: clean

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $+
	$(STRIP) $(NAME)

clean:
	rm -f $(NAME) $(OBJS)
