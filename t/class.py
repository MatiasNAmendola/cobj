#!/usr/bin/env python

from TAP.Simple import *

plan("no_plan")

test_expect_result("""<class 'Complex'>
""", """
class Complex
end
print(Complex)
""")

test_expect_result("""attr
""", """
class Complex
end
c = Complex()
c.attr = "attr"
print(c.attr)
""")

test_expect_result("""20
20
""", """
class Complex
end
Complex.real = 10
Complex.image = 10
Complex.add = func(self, other)
    local c = Complex()
    c.real = self.real + other.real
    c.image = self.image + other.image
    return c
end

c1 = Complex()
c2 = Complex()

c = c1:add(c2)
print(c.real)
print(c.image)
""")
