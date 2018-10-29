from glob import glob
from os.path import abspath, dirname, join, basename
import re
from subprocess import check_output
import unittest as ut


class EmitterTest(ut.TestCase):
    IS_WHITESPACE_RE = re.compile('^\s*$')
    INPUT_RE = re.compile('^\s*;\s*INPUT\s*:\s*(.+?)\s*$')

    def test_emitter(self):
        testdir = abspath(dirname(__file__))
        for input_file in glob(join(testdir, 'EmitterTest/*.ll')):
            test_name = basename(input_file).rsplit('.', 1)[0]
            with self.subTest(test_name):
                self.emitter_test_file(input_file)

    def emitter_test_file(self, input_file):
        with open(input_file, 'r') as fp:
            command = None

            for line in fp.readlines():
                if self.IS_WHITESPACE_RE.match(line):
                    continue
                elif self.INPUT_RE.match(line):
                    line = self.INPUT_RE.match(line)
                    command = line.groups(0)[0]
                    print('TEST CASE: ' + command)
                    self.assertTrue(False)
                elif not command:
                    self.fail(input_file + ' did not specify a test command')
                else:
                    pass


if __name__ == '__main__':
    ut.main()
