#!/usr/bin/env python
# coding: utf-8
from TAP.Simple import *

plan("no_plan")

test_expect_result("""Hello, World!
""", """
a = "Hello"
b = "World"
print(a + ", " + b + "!")
""")

