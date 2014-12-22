OBJDIR=objs
SRCDIR=src
INCDIR=$(SRCDIR)/inc
CFLAGS+=-I$(INCDIR)

CC=gcc

SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

CFLAGS+=-O2 -Wall -Werror
LDFLAGS+=-lncurses -pthread

all: game

run: game
	./game

game:	$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(OBJS): | $(OBJDIR)
$(OBJDIR):
	mkdir -p $@ && echo $(OBJS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INCDIR)/*.h Makefile
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -rf $(OBJDIR) game