#!/usr/bin/env python3
########################################################################
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
########################################################################

import unittest
import os
from orcus import xlsx


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        # base directory for xlsx test files.
        basedir = os.path.dirname(__file__) + "/../xlsx/"
        cls.basedir = os.path.normpath(basedir)

    def test_raw_values_1(self):
        filepath = self.basedir + "/raw-values-1/input.xlsx"
        doc = xlsx.read_file(filepath)
        self.assertEqual(len(doc.sheets), 2)
        self.assertEqual(doc.sheets[0].name, "Num")
        self.assertEqual(doc.sheets[1].name, "Text")
        self.assertEqual(doc.sheets[0].data_size, {"column": 5, "row": 7})
        self.assertEqual(doc.sheets[1].data_size, {"column": 4, "row": 10})

        # expected sheet contents (None == empty cell)
        sheet_contents = (
            (
                (None, None, None, None, None),
                ( 1.1, None,  2.1, None,  3.1),
                ( 1.2, None,  2.2, None,  3.2),
                ( 1.3, None,  2.3, None,  3.3),
                (None, None, None, None, None),
                (None, None, None, None, None),
                (None, None,  5.0,  6.0,  7.0)
            ),
            (
                ( 'A', None, None,      None),
                ( 'B',  'D', None,      None),
                ( 'C',  'E',  'G',      None),
                (None,  'F',  'H',      None),
                (None, None,  'I',      None),
                (None, None, None,      None),
                (None, None, None,    'Andy'),
                (None, None, None,   'Bruce'),
                (None, None, None, 'Charlie'),
                (None, None, None,   'David')
            )
        )

        for sheet, sheet_content in zip(doc.sheets, sheet_contents):
            rows = sheet.get_rows()
            for row, expected in zip(rows, sheet_content):
                self.assertEqual(row, expected)


if __name__ == '__main__':
    unittest.main()
