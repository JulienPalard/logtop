#!/usr/bin/env python3

import pprint
import sys
from logtop import logtop


"""
This example show a complete usage of the python API,
It can be used like this:

$ make python-module
$ python3 -m venv logtop_venv
$ . logtop_venv/bin/activate
$ python3 -m pip install .
$
$ cat /etc/passwd | cut -d: -f7 | python3 examples/example1.py

"""

l = logtop(10000)
for line in sys.stdin:
    l.feed(line)

pprint.pprint(l.get(10))
