#!/usr/bin/env python
#
# Test special cases for syntax consistency.
#

from TAP.Simple import *

plan("no_plan")

test_expect_result("", "", "allow empty code");
test_expect_result("", """func empty() {}""", "allow empty function");
