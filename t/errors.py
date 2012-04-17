#!/usr/bin/env python
from TAP.Simple import *

plan("no_plan")

test_expect_result("NameError: name 'unknown' is not defined\n", "print unknown;")

test_expect_result(r"""no catch and no finally
""", r"""
try
    print "no catch and no finally"
end
""", "no catch, no finally")

test_expect_result(r"""try-catch: try
""", r"""
try
    print "try-catch: try"
catch
    print "try-catch: catch"
end
""")

test_expect_result(r"""try-catch: try
try-catch: catch
""", r"""
try
    print "try-catch: try"
    throw
    print "try-catch: after throw"
catch
    print "try-catch: catch"
end
""")

test_expect_result(r"""inner
outer
""", r"""
try
    try
        throw
    catch
        print "inner"
        throw
    end
catch
    print "outer"
end""")
