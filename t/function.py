#!/usr/bin/env python
from TAP.Simple import *

plan(3)

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

test_expect_result("""outside a
""", """
a = "outside a";
str = "outside str";
func hello(str) {
    print str;
}

hello(a);
""")

test_expect_result("""0
1
1
2
3
5
8
13
21
34
""", """
func fibo(num) {
    if (num == 0) {
        return 0;
    } else if (num == 1) {
        return 1;
    } else {
        return fibo(num - 1) + fibo(num - 2);
    }
}

i = 0;
while (i < 10) {
    print fibo(i);
    i = i + 1;
}
""")
