#!/usr/bin/env python
from TAP.Simple import *

plan("no_plan")

test_expect_result("""if
else if
else
""", """
num = 10;
if (num < 100) {
    print "if";
}

if (num == 1) {
    print "if";
} else if (num == 10) {
    print "else if";
} else {
    print "else";
}

if (num == 0) {
    print "if";
} else if (num == 1) {
    print "else if";
} else {
    print "else";
}
""")
