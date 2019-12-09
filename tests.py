#!/usr/bin/python3

import unittest
import subprocess

FILE_NAME = './fcopy'
PYTHON_FILE = './fcopy.py'

class TestCat(unittest.TestCase):

    def test_run_without_argument(self):
        expected = subprocess.run([PYTHON_FILE])
        actual = subprocess.run([FILE_NAME])
        self.assertEqual(expected.returncode, actual.returncode)

    def test_run_without_output_file(self):
        expected = subprocess.run([PYTHON_FILE, 'text1', 'text2'])
        actual = subprocess.run([FILE_NAME, 'text1', 'text2'])
        self.assertEqual(expected.returncode, actual.returncode)

    def test_run_without_input_file(self):
        expected = subprocess.run([PYTHON_FILE], env={'OUT_FILE_NAME': 'out'})
        actual = subprocess.run([FILE_NAME], env={'OUT_FILE_NAME': 'out'})
        self.assertEqual(expected.returncode, actual.returncode)

    def test_run_with_empty_string_name_output_file(self):
        expected = subprocess.run([PYTHON_FILE, 'text1', 'text2'], env={'OUT_FILE_NAME': ''})
        actual = subprocess.run([FILE_NAME, 'text1', 'text2'], env={'OUT_FILE_NAME': ''})
        self.assertEqual(expected.returncode, actual.returncode)

    def test_open_out_file_error(self):
        expected = subprocess.run([PYTHON_FILE, 'text1', 'text2'], env={'OUT_FILE_NAME': 'error/file'})
        actual = subprocess.run([FILE_NAME, 'text1', 'text2'], env={'OUT_FILE_NAME': 'error/file'})
        self.assertEqual(expected.returncode, actual.returncode)

    def test_no_input_file(self):
        expected = subprocess.run([PYTHON_FILE, 'text1', 'no_such_file'], env={'OUT_FILE_NAME': 'expected_out_1'})
        actual = subprocess.run([FILE_NAME, 'text1', 'no_such_file'], env={'OUT_FILE_NAME': 'actual_out_1'})
        self.assertEqual(expected.returncode, actual.returncode)
        with open('expected_out_1') as f1:
            with open('actual_out_1') as f2:
                self.assertEqual(f1.read(), f2.read())

    # def test_no_permitions_in_input_file(self):
    #     expected = subprocess.run([PYTHON_FILE, 'text1', 'no_permition'], env={'OUT_FILE_NAME': 'expected_out_2'})
    #     actual = subprocess.run([FILE_NAME, 'text1', 'no_permition'], env={'OUT_FILE_NAME': 'actual_out_2'})
    #     self.assertEqual(expected.returncode, actual.returncode)
    #     with open('expected_out_2') as f1:
    #         with open('actual_out_2') as f2:
    #             self.assertEqual(f1.read(), f2.read())

    def test_work_normally(self):
        expected = subprocess.run([PYTHON_FILE, 'text1', 'text2'], env={'OUT_FILE_NAME': 'expected_out_3'})
        actual = subprocess.run([FILE_NAME, 'text1', 'text2'], env={'OUT_FILE_NAME': 'actual_out_3'})
        self.assertEqual(expected.returncode, actual.returncode)
        with open('expected_out_3') as f1:
            with open('actual_out_3') as f2:
                self.assertEqual(f1.read(), f2.read())


if __name__ == "__main__":
    unittest.main()
