#!/usr/bin/env python
from TAP.Simple import *

plan("no_plan")

test_expect_result("""hello world
hello,
world
hello,
10
""", """
func helloworld
    print "hello world"
end

func hello(str) 
    print "hello,"
    print str
end

helloworld()
hello("world")
hello(10)
""")

test_expect_result("""outside a
""", """
a = "outside a"
str = "outside str"
func hello(str)
    print str
end

hello(a)
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
func fibo(num)
    if (num == 0) {
        return 0
    } else if (num == 1) {
        return 1
    } else {
        return fibo(num - 1) + fibo(num - 2)
    }
end

i = 0
while (i < 10) {
    print fibo(i)
    i = i + 1
}
""", "recursive function")

test_expect_result("""1
1
2
6
24
""", """
f = func(n)
    if (n == 0) {
        return 1
    } else {
        return n * f(n - 1)
    }
end

i = 0
while (i < 5) {
    print f(i)
    i = i + 1
}
""", "closures")

test_expect_result("""10
11
13
""", """

newIncrer = func(n)
    return func(i)
        n = n + i
        return n
    end
end

f = newIncrer(10)
i = 0
while (i < 3) {
    print f(i)
    i = i + 1
}
""", "closures")

test_expect_result("""1
2
3
6
""", """
func sum(a, b, c)
    print a
    print b
    print c
    return a + b + c
end
print sum(1, 2, 3)
""")
