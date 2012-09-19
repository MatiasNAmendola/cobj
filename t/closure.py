#!/usr/bin/env python
from TAP.Simple import *

plan("no_plan")

#test_expect_result("""21
#33
#""", """
#local base = 10
#newIncrer = func(n)
#    return func(i)
#        n = n + base + i
#        return n
#    end
#end
#f = newIncrer(10)
#print(f(1))
#print(f(2))
#""", "more than one level local stacks")


test_expect_result("""12
13
""", """
newIncrer = func(n)
    return func(i) 
        n = n + i
        return func(j)
            n = n + j
            return n
        end
    end
end

f = newIncrer(10)
f = f(1)
print(f(1))
print(f(1))
""", "upvalues in upvalues")
