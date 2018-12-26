# Install

## Debian / Debian based

```
apt install logtop
```


## Void Linux

```
xbps-install -S logtop
```


## From sources

Compile dependencies: `libncursesw5-dev`, `uthash-dev`.

So on a Debian:

```bash session
$ sudo apt install libncursesw5-dev uthash-dev
Reading package lists... Done
[...skipped for readability...]
Processing triggers for man-db (2.8.2-1) ...
$ make
gcc -c -O3 -DVERSION=0.7.0 -Wall -fPIC -Wextra -ansi -pedantic -Wstrict-prototypes -I. src/avl.c -o src/avl.o
[...skipped for readability...]
$ make install
```

# Usage

`logtop` displays a real-time count of strings received from stdin.
It can be useful in some cases, like getting the IP flooding your server:

```bash session
$ tail -f /var/log/apache2/access.log | cut -d' ' -f1 | logtop
```

Or the top buzzing article of your blog:

```bash session
$ tail -f /var/log/apache2/access.log | cut -d' ' -f7 | grep article | logtop
```


# Python bindings

Dependencies: `python-dev`, `swig`.

```bash session
$ sudo apt install python-dev swig
Reading package lists... Done
[...]
$ make python-module
[...]
$ python setup.py install
```


# Development

I use a hashtable to store strings and an AVL tree to store frequencies,
so I can fetch by string or fetch ordered by frequency to display the
top-strings.


# C API

Logtop can be used by your C programs, you may to compile against
logtop's sources (`src/{avl.c,history.c,logtop.c,libavl/avl.c}`) or
against `liblogtop`, obtained using `make liblogtop`.

C API is described in `logtop.h`, you need:

```C
struct logtop *new_logtop(size_t history_size);
void delete_logtop(struct logtop *this);
void logtop_feed(struct logtop *this, char *line);
struct logtop_state *logtop_get(struct logtop *this, size_t qte);
double logtop_timespan(struct logtop *this);
unsigned int logtop_qte_of_elements(struct logtop *this);
```

You can find an example of using the C API in `examples/example1.c`.


# Python API

`logtop` module exposes a logtop class containing:

```Python
logtop.__init__(history_size)  # to build a new logtop keeping
                               # at most history_size lines.
logtop.feed(line)  # to feed a new line in logtop.
logtop.get(qte_of_elements)   # to get the top qte_of_elements lines.
logtop.qte_of_elements()  # to get the current total number of lines.
logtop.timespan()  # to get the duration from the oldest line to now.
```

timespan may be less than the runtime, as logtop drop old lines,
to keep, at most, `history_size` lines, given in the constructor of
the logtop class.


# About libavl

The libavl used here is the Ben Pfaff's one, statically build with logtop, as
Ben want it to be (see INSTALL file and here:
http://lists.debian.org/debian-devel/2001/07/msg01303.html)
So this libavl is NOT packaged as a library for Debian, the libavl you'll
found in Debian repositories is the Wessel Dankers's one.


# About the project

For copyright information, please see the file COPYRIGHT in this
directory or in the files of the source tree.

This project was initiated on 2010-06-29 by Palard Julien
See http://julien.palard.fr or ask me questions at :
julien at palard dot fr.
