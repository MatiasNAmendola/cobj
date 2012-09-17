#!/usr/bin/env python
from TAP.Simple import *

plan("no_plan")

test_expect_result("""1 2 3
1 2 3
3 4 5
""",
"""
a, b, c = (1, 2, 3)
print(a, b, c)
a, b, c = [1, 2, 3]
print(a, b, c)
a, b, c = range(3, 6)
print(a, b, c)
""");
