#!/usr/bin/env python

import sys
import os.path as path
sys.path.append(path.realpath(__file__ + '/../../'))

from threading import Thread, Lock
import time
from logtop import logtop  # use make python-module to build it


def build_lines():
    while True:
        for i in range(10):
            yield str(i % 3)
            time.sleep(1)
        time.sleep(10)


class LogtopPrinter(Thread):
    def __init__(self):
        self.lock = Lock()
        self.logtop = logtop(10000)
        Thread.__init__(self)
        self.daemon = True

    def run(self):
        while True:
            self.lock.acquire()
            data = self.logtop.get(6)
            count = self.logtop.qte_of_elements()
            timespan = self.logtop.timespan()
            self.lock.release()
            if timespan == 0:
                timespan = 1
            print "%.2f l/s (%d lines, %.2f seconds): %s" % (count / timespan,
                                                             count,
                                                             timespan,
                                                             repr(data))
            time.sleep(1)

    def feed(self, line):
        self.lock.acquire()
        self.logtop.feed(line)
        self.lock.release()


printer = LogtopPrinter()
printer.start()

for line in build_lines():
    printer.feed(line)
