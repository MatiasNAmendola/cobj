#!/usr/bin/env python
# coding: utf-8
from TAP.Simple import *

plan("no_plan")

# None
test_expect_result("None\n", "print(None)")

# Bool
test_expect_result("True\n", "print(True)")
test_expect_result("False\n", "print(False)")

# Int
test_expect_result("0\n", "print(0)", "Int")
test_expect_result("-123456789012345678901234567890\n", "print(-123456789012345678901234567890)");
test_expect_result("123456789012345678901234567890\n", "print(123456789012345678901234567890)");
test_expect_result("31\n", "print(0b11111)");
test_expect_result("1234567890\n", "print(0o11145401322)");
test_expect_result("255\n", "print(0xFF)", "Int")
test_expect_result("255\n", "print(0XFF)", "Int")
test_expect_result("255\n", "print(0Xff)", "Int")

# Float
test_expect_result("3.14\n", "print(3.14)")


# String
test_expect_result("\n", """print('')""")
test_expect_result("\n", """
print()
""")
test_expect_result(u"UTF-8字符串\n", u"print('UTF-8字符串')")
test_expect_result(r"don't escape \a, except single quote (') and backslash (\)" + "\n", r"print ('don\'t escape \\a, except single quote (\') and backslash (\\)')", "single quoted string")
test_expect_result(r"""
hello world after newline
""", r"""print ('
hello world after newline');
""")
test_expect_result("\a\b\f\n\r\t\v\\" + "\n", r'print("\a\b\f\n\r\t\v\\")')
test_expect_result('hello "world"\n', r'print("hello \"world\"")')
test_expect_result("a\n", r'print("\141")')
test_expect_result("a\n", r'print("\x61")')
test_expect_result(""" span
    many
    lines double-
    quoted string
""" + "\n", r'''print(" span
    many
    lines double-
    quoted string
")
''')
test_expect_result("", """
d = {
    'key' => 'value',
    'c' => 'test',
    0 => "test integer",
}""")
