##
## Makefile for logtop
##
## Made by julien palard
## Login   <logtop@mandark.fr>
##

NAME = logtop
DSRC = src
SRC = $(DSRC)/logtop.c $(DSRC)/avl.c $(DSRC)/history.c $(DSRC)/curses.c $(DSRC)/stdout.c
OBJ = $(SRC:.c=.o)
CC = gcc
INCLUDE = .
DEFINE = _GNU_SOURCE
LIB = -lavl -lncurses #-lefence
CFLAGS = -fPIC -g3 -O3 -W -Wall -ansi -pedantic -I$(INCLUDE)
RM = rm -f

$(NAME):	$(OBJ)
		$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(LIB)

install:	$(NAME)
		mkdir -p $(DESTDIR)/usr/bin/
		cp $(NAME) $(DESTDIR)/usr/bin/

all:
		@make $(NAME)

.c.o:
		$(CC) -D $(DEFINE) -c $(CFLAGS) $< -o $(<:.c=.o)

clean:
		$(RM) $(NAME) $(DSRC)/*~ $(DSRC)/#*# $(DSRC)/*.o $(DSRC)/*.core

re:		clean all
