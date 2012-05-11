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
