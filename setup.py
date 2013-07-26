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
                                    'src/libavl/avl.c']
                           )

setup (name = 'logtop',
       version = '0.2',
       author      = "Julien Palard",
       description = """Live line frequency analyzer""",
       ext_modules = [logtop_module],
       py_modules = ["logtop"],
       )
