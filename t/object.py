#!/usr/bin/env python
#
# Test special cases for syntax consistency.
#

from TAP.Simple import *

plan("no_plan")

test_expect_result("""4
""", """
print(10:bit_length())
""")
