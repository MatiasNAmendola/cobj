#!/usr/bin/env python
# coding: utf-8
from TAP.Simple import *

plan("no_plan")

test_expect_result("None\n", "print None;")
test_expect_result("True\n", "print True;")
test_expect_result("False\n", "print False;")
test_expect_result("10\n", "print 10;")
test_expect_result("3.14\n", "print 3.14;")
test_expect_result(u"UTF-8字符串\n", u"print 'UTF-8字符串';")
test_expect_result(r"don't escape \a, except single quote (') and backslash (\)" + "\n", r"print 'don\'t escape \\a, except single quote (\') and backslash (\\)';", "single quoted string")
test_expect_result(r"""
hello world after newline
""", r"""print '
hello world after newline';
""")
test_expect_result("\a\b\f\n\r\t\v\\" + "\n", r'print "\a\b\f\n\r\t\v\\";')
test_expect_result('hello "world"\n', r'print "hello \"world\"";')
test_expect_result("a\n", r'print "\141";')
test_expect_result("a\n", r'print "\x61";')
test_expect_result(""" span
    many
    lines double-
    quoted string
""" + "\n", r'''print " span
    many
    lines double-
    quoted string
";
''')
