#!/usr/bin/env python
from TAP.Simple import *

plan(1)

test_expect_result("""hello world
hello,
world
hello,
10
""", """func helloworld() {
    print "hello world";
}

func hello(str) {
    print "hello,";
    print str;
}

helloworld();
hello("world");
hello(10);
""")
