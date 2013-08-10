#!/usr/bin/env python

import pprint
import sys
sys.path.append('..')

from logtop import logtop


"""

This example show a complete usage of the python API,
It can be used like this :

$ make python-module
$ cat /etc/passwd | cut -d: -f7 | python example1.py

"""

l = logtop(10000)
for line in sys.stdin:
    l.feed(line)

pprint.pprint(l.get(10))
