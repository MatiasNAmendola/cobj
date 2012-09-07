#!/usr/bin/env python
#
# Test special cases for syntax consistency.
#

from TAP.Simple import *

plan("no_plan")

test_expect_result("""A data string
A data string
A data string
A data string
A data string
20
""", """
o = {
    "data" => "A data string",
    "balance" => 10,
    "foo" => func(self) return self.data end,
    "draw" => func(self, a) self.balance = self.balance + a; return self.balance end
}
print(o["data"])
print(o.data)
print(o["foo"](o))
print(o.foo(o))
print(o:foo())
print(o:draw(10))
""")
