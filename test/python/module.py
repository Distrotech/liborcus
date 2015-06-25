#!/usr/bin/env python3

import unittest
import orcus


class ModuleTest(unittest.TestCase):

    def test_info(self):
        orcus.info()


if __name__ == '__main__':
    unittest.main()
