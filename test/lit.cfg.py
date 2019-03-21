import os
import os.path
import sys
from tempfile import mkdtemp
import lit.formats

config.name = 'llvmPy'
config.suffixes = ['.py', '.ll']
config.excludes = ['lit.cfg.py']
config.test_source_root = os.path.join(os.path.dirname(__file__), 'src')
config.environment['PATH'] = \
    os.path.join(config.test_source_root, '..', 'tools') + \
    os.pathsep + config.environment['PATH']
config.test_exec_root = mkdtemp('llvmPy-lit-')
config.test_format = lit.formats.ShTest(execute_external=True)
