#!/usr/bin/env python
from TAP.Simple import *

plan(13)

test_expect_result("1\n", "print 0 + 1;")
test_expect_result("2\n", "print 3 - 1;")
test_expect_result("3\n", "print 3 * 1;")
test_expect_result("4\n", "print 100 / 25;")
test_expect_result("5\n", "print 124 % 7;")
test_expect_result("True\n", "print 3 < 10;")
test_expect_result("False\n", "print 3 > 10;")
test_expect_result("6\n", "num = 6; print num;", "assign")
test_expect_result("7\n", "print 1 + 2 + 3 + 1;", "nested expression")
test_expect_result("8\n", "print (1 + 2 + 3 + 4 + 7) - 9;", "parenthesized expression")
test_expect_result("9\n", "print 1 + 64 / 8;", "precedence & association 1")
test_expect_result("10\n", "print 18 - 1 * 8;", "precedence & association 2")
test_expect_result("11\n", "print (25 - 1 - 1) % 12;", "precedence & association 3")
