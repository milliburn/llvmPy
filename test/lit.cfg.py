import os
import sys
from tempfile import mkdtemp
import lit.formats

config.name = 'llvmPy'
config.suffixes = ['.ll']
config.test_source_root = os.path.dirname(__file__)
config.test_exec_root = mkdtemp('llvmPy-lit-')
config.test_format = lit.formats.ShTest(execute_external=False)
