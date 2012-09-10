#!/usr/bin/env python
from TAP.Simple import *

plan("no_plan")

# objects
test_expect_result("[]\n", "print([])");
test_expect_result("[1, 2, 3]\n", "print([1,2,3])");

# object binary operations
# add
test_expect_result("1\n", "print(0 + 1)")
# sub
test_expect_result("2\n", "print(3 - 1)")
# mul
test_expect_result("121932631112635269\n", "print(123456789 * 987654321)")
# div
test_expect_result("4\n", "print(100 / 25)")
# mod
test_expect_result("-10000\n", "print(-111111111111111111111111/11111111111111111111)")
# unary_negate
test_expect_result("-10\n", "print(-10)");
# unary_invert
test_expect_result("-11\n", "print(~10)");

test_expect_result("1\n", "print(4 >> 2)")
test_expect_result("4\n", "print(1 << 2)")
test_expect_result("True\n", "print(3 < 10)")
test_expect_result("False\n", "print(3 > 10)")
test_expect_result("True\n", "print(3 == 3)")
test_expect_result("True\n", "print(3 != 10)")
test_expect_result("True\n", "print(3 <= 10)")
test_expect_result("False\n", "print(3 >= 10)")


test_expect_result("6\n", "num = 6; print(num)", "assign")
test_expect_result("8\n", "num = 6; num += 2; print(num)", "add assign")
test_expect_result("4\n", "num = 6; num -= 2; print(num)", "sub assign")
test_expect_result("12\n", "num = 6; num *= 2; print(num)", "mul assign")
test_expect_result("3\n", "num = 6; num /= 2; print(num)", "div assign")
test_expect_result("1\n", "num = 6; num %= 5; print(num)", "mod assign")
test_expect_result("2\n", "num = 4; num >>= 1; print(num)")
test_expect_result("8\n", "num = 2; num <<= 2; print(num)")
test_expect_result("7\n", "print(1 + 2 + 3 + 1)", "nested expression")
test_expect_result("8\n", "print((1 + 2 + 3 + 4 + 7) - 9)", "parenthesized expression")
test_expect_result("9\n", "print(1 + 64 / 8)", "precedence & association 1")
test_expect_result("10\n", "print(18 - 1 * 8)", "precedence & association 2")
test_expect_result("11\n", "print((25 - 1 - 1) % 12)", "precedence & association 3")


# 'in', 'not in', 'is', 'is not'
test_expect_result("True\n", "print(1 in (1,2,3))")
test_expect_result("True\n", "print(1 not in (2,3))")
test_expect_result("True\n", "print(builtins.str is str)")
test_expect_result("True\n", "print(builtins.str is not print)")
