#!/usr/bin/env python
from TAP.Simple import *

plan("no_plan")

test_expect_result("""2
3
5
7
11
13
17
19
23
29
31
37
41
43
47
53
59
61
67
71
73
79
83
89
97
""", r"""
func is_prime(n)
    j = 2
    while j * j < n + 1
        if n % j == 0
            return False
        end
        j += 1
    end
    return True
end


i = 2
while i <= 100
    if is_prime(i)
        print(i)
    end
    i += 1
end
""")
