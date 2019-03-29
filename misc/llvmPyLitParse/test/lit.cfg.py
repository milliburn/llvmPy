import os
import sys
from tempfile import mkdtemp
import lit.formats

config.name = 'llvmPyLitParser'
config.suffixes = ['.txt']
config.excludes = ['lit.cfg.py']
config.test_source_root = os.path.dirname(__file__)
config.test_exec_root = mkdtemp('llvmPyLitParser-lit-')
config.test_format = lit.formats.ShTest(execute_external=True)
