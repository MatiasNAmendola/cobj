#!/usr/bin/env python
from TAP.Simple import *

plan("no_plan")

test_expect_result(r"""1
2
3
a
b
c
a
b
c
""", r'''
for i in [1,2,3]
    print(i)
end
for i in ['a', 'b', 'c']
    print(i)
end
for k in {"a" => 1, "b" => 2, "c" => 3}
    print(k)
end
''', "iterator")
