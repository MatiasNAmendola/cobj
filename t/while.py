#!/usr/bin/env python
from TAP.Simple import *

plan("no_plan")

test_expect_result(r"""10
""", r'''
i = 1

while i < 10 
    i = i + 1
end
print i

while False 
    print i
    print "hello world"
end
''')

test_expect_result(r"""1
2
3
""", r'''
i = 1

while i < 10 
    if i == 4 then break end
    print i
    i += 1
end
''')

test_expect_result(r"""2
3
5
6
7
8
9
10
""", r'''
i = 1

while i < 10 
    i += 1
    if i == 4 then continue end
    print i
end
''')
