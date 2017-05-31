#!/usr/bin/env python

"""
setup.py file for logtop
"""

from distutils.core import setup, Extension


logtop_module = Extension('_logtop',
                          sources=['logtop_wrap.c',
                                   'src/logtop.c',
                                   'src/avl.c',
                                   'src/history.c',
                                   'src/libavl/avl.c'])

setup(name='logtop',
      version='0.2',
      author="Julien Palard",
      author_email="julien@palard.fr",
      description="""Live line frequency analyzer""",
      long_description="""logtop is a System Administrator tool analyzing line
       rate on stdin. It reads on stdin and print a constantly updated result
       displaying, in columns: Line number, count, frequency,
       and the actual line.""",
      keywords=['log', 'top', 'lines', 'analyzer', 'nginx', 'apache',
                'varnish', 'tail'],
      url='https://github.com/JulienPalard/logtop',
      classifiers=['Development Status :: 5 - Production/Stable',
                   'Intended Audience :: System Administrators',
                   'License :: OSI Approved :: BSD License'],
      ext_modules=[logtop_module],
      py_modules=["logtop"])
