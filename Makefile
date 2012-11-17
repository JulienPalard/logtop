##
## Makefile for logtop
##
## Made by julien palard
## Login   <logtop@mandark.fr>
##

NAME = logtop
DSRC = src
SRC = $(DSRC)/logtop.c $(DSRC)/avl.c $(DSRC)/history.c $(DSRC)/curses.c \
      $(DSRC)/stdout.c $(DSRC)/libavl/avl.c
OBJ = $(SRC:.c=.o)
CC = gcc
INCLUDE = .
LIB = -lncurses #-lefence
CFLAGS = -O3 -Wall -Wextra -ansi -pedantic -Wstrict-prototypes -I$(INCLUDE)
RM = rm -f
LDFLAGS =

$(NAME):	$(OBJ)
		$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(LIB) $(LDFLAGS)

install:	$(NAME)
		mkdir -p $(DESTDIR)/usr/bin/
		cp $(NAME) $(DESTDIR)/usr/bin/

all:
		@make $(NAME)

.c.o:
		$(CC) -c $(CFLAGS) $< -o $(<:.c=.o)

clean:
		$(RM) $(NAME) $(DSRC)/*~ $(DSRC)/#*# $(DSRC)/*.o $(DSRC)/*.core \
				$(DSRC)/libavl/*.o

re:		clean all

check-syntax:
		gcc -Isrc -Wall -Wextra -ansi -pedantic -o /dev/null -S ${CHK_SOURCES}
