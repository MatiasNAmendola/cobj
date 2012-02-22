#!/usr/bin/env python
from TAP.Simple import *

plan(1)

test_expect_result(r"""10
""", r'''
i = 1;

while (i < 10) {
    i = i + 1;
}
print i;

while (False) {
    print i;
    print "hello world";
}
''')
