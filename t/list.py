#!/usr/bin/env python
# coding: utf-8
from TAP.Simple import *

plan("no_plan")

test_expect_result("""1
2
3
3
""", """
l = [1,2,3,4]
l[3] = 3
for i in l
    print(i)
end
""")
