#!/usr/bin/env python
# coding: utf-8
from TAP.Simple import *

plan("no_plan")

test_expect_result("""a
""", """
d = {
    "string": "a",
    10: "10",
}
print(d["string"])
#print(d.string)
""")

test_expect_result("""10
10
11
11
12
12
""", """
dict = {
    "a": 10
}
print(dict.a)
print(dict["a"])
dict["a"] = 11
print(dict.a)
print(dict["a"])
dict.a = 12
print(dict.a)
print(dict["a"])
""")

test_expect_result("""{}
""", """
dict = {
}
print(dict)
""", "empty dict")
