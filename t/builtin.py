#!/usr/bin/env python
#

from TAP.Simple import *

plan("no_plan")

# print
test_expect_result("10\n", "print(10)")

# str
test_expect_result("10\n", "print(str(10))")

# type
test_expect_result("10\n", "print(type('1')(10))")
