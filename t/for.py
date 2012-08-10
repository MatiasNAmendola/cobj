#!/usr/bin/env python
from TAP.Simple import *

plan("no_plan")

test_expect_result(r"""1
2
3
4
a
b
c
""", r'''
l = [1,2,3,4,"a", "b", "c"]

for i in l
    print(i)
end
''')
