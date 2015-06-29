#!/usr/bin/env python3
########################################################################
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
########################################################################

import unittest
import orcus


class ModuleTest(unittest.TestCase):

    def test_info(self):
        orcus.info()


if __name__ == '__main__':
    unittest.main()
