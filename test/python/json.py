#!/usr/bin/env python3
########################################################################
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
########################################################################

import unittest
from orcus import json


class JsonTest(unittest.TestCase):

    def test_loads(self):
        s = '[1,2,3,"foo",[4,5,6], {"a": 12.3, "b": 34.4, "c": [true, false, null]}]'
        o = json.loads(s)
        self.assertTrue(isinstance(o, list))
        self.assertEqual(len(o), 6)
        self.assertEqual(o[0], 1)
        self.assertEqual(o[1], 2)
        self.assertEqual(o[2], 3)
        self.assertEqual(o[3], "foo")

        self.assertTrue(isinstance(o[4], list))
        self.assertEqual(o[4][0], 4)
        self.assertEqual(o[4][1], 5)
        self.assertEqual(o[4][2], 6)

        d = o[5]
        self.assertTrue(isinstance(d, dict))
        self.assertEqual(len(d), 3)
        self.assertEqual(d["a"], 12.3)
        self.assertEqual(d["b"], 34.4)

        l = d["c"]
        self.assertEqual(len(l), 3)
        self.assertEqual(l[0], True)
        self.assertEqual(l[1], False)
        self.assertEqual(l[2], None)


if __name__ == '__main__':
    unittest.main()
