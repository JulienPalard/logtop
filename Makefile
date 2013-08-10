##
## Makefile for logtop
##
## Made by julien palard
## Login   <logtop@mandark.fr>
##

VERSION = 0.6.1
MINOR = 0
RELEASE = 0

NAME = logtop
LINKERNAME = lib$(NAME).so
SONAME=$(LINKERNAME).$(VERSION)
REALNAME=$(SONAME).$(MINOR).$(RELEASE)

LIB_SRC = src/avl.c src/history.c \
          src/logtop.c src/libavl/avl.c
SRC = $(LIB_SRC) src/curses.c src/stdout.c src/main.c

LIB_OBJ = $(LIB_SRC:.c=.o)
OBJ = $(SRC:.c=.o)

CC = gcc
INCLUDE = .
LIB = -lncurses #-lefence
CFLAGS = -O3 -DVERSION=$(VERSION) -Wall -fPIC -Wextra -ansi -pedantic -Wstrict-prototypes -I$(INCLUDE)
RM = rm -fr
LDFLAGS =

$(NAME):	$(OBJ)
		$(CC) -o $(NAME) $(OBJ) $(LIB) $(LDFLAGS)

lib$(NAME): $(LIB_OBJ)
		$(CC) --shared -o $(LINKERNAME) $(LIB_OBJ) $(LDFLAGS)

install:	$(NAME)
		mkdir -p $(DESTDIR)/usr/bin/
		cp $(NAME) $(DESTDIR)/usr/bin/

python-module:
		swig -python *.i
		python ./setup.py build_ext --inplace

all:
		@make $(NAME)
		@make lib$(NAME)

.c.o:
		$(CC) -c $(CFLAGS) $< -o $(<:.c=.o)

clean:
		$(RM) $(NAME) src/*~ src/#*# src/*.o src/*.core \
				src/libavl/*.o _logtop.so liblogtop.so \
				logtop.py *.pyc build/ logtop_wrap.c

re:		clean all

check-syntax:
		gcc -Isrc -Wall -Wextra -ansi -pedantic -o /dev/null -S ${CHK_SOURCES}
