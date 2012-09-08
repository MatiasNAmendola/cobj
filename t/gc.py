#!/usr/bin/env python
#
# Test special cases for syntax consistency.
#

from TAP.Simple import *

plan("no_plan")

test_expect_result("""6
""", """
func hello()
    local a = [None, [1,2,3]]
    local b = [a]
    local c = [b]
    local d = [c]
    local e = [d]
    a[0] = e
end

hello()
print(gc.collect())
""")
