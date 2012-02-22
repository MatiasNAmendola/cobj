#!/usr/bin/env python
from TAP.Simple import *

plan(1)

test_expect_result("""1
2
3
""", """
num = 10;
if (num > 100) {
    print 1;
}

if (1) {
    print 1;
    print 2;
    print 3;
} else if (0) {
    print 3;
    print 2;
    print 1;
} else {
    print 0;
}
""")
