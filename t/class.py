#!/usr/bin/env python

from TAP.Simple import *

plan("no_plan")

test_expect_result("""<class 'Complex'>
""", """
class Complex
end
print(Complex)
""")
