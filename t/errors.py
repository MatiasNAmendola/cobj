#!/usr/bin/env python
from TAP.Simple import *

plan("no_plan")

test_expect_result("NameError: name 'unknown' is not defined\n", "print(unknown)")

test_expect_result(r"""no catch and no finally
""", r"""
try
    print("no catch and no finally")
end
""", "no catch, no finally")

test_expect_result(r"""SystemError: a
""", r"""
try
    throw "a"
end""", "no catch, no finally, throw")

test_expect_result(r"""try-catch: try
try-catch: catch a
""", r"""
try
    print("try-catch: try")
    throw "a"
    print("try-catch: after throw")
catch "a"
    print("try-catch: catch a")
catch "b", "c"
    print("try-catch: catch b or c")
catch
    print("try-catch: default catch")
end
""")

test_expect_result(r"""try-catch: try
try-catch: catch b or c
""", r"""
try
    print("try-catch: try")
    throw "b"
    print("try-catch: after throw")
catch "a"
    print("try-catch: catch a")
catch "b", "c"
    print("try-catch: catch b or c")
catch
    print("try-catch: default catch")
end
""")

test_expect_result(r"""try-catch: try
try-catch: catch b or c
""", r"""
try
    print("try-catch: try")
    throw "c"
    print("try-catch: after throw")
catch "a"
    print("try-catch: catch a")
catch "b", "c"
    print("try-catch: catch b or c")
catch
    print("try-catch: default catch")
end
""")

test_expect_result(r"""try-catch: try
try-catch: default catch
""", r"""
try
    print("try-catch: try")
    throw "d"
    print("try-catch: after throw")
catch "a"
    print("try-catch: catch a")
catch "b", "c"
    print("try-catch: catch b or c")
catch
    print("try-catch: default catch")
end
""")

test_expect_result(r"""try-catch: try
try-catch: catch
""", r"""
try
    print("try-catch: try")
    throw
    print("try-catch: after throw")
catch
    print("try-catch: catch")
end
""")

test_expect_result(r"""try-catch: try
try-catch: catch
finally
""", r"""
try
    print("try-catch: try")
    throw
    print("try-catch: after throw")
catch
    print("try-catch: catch")
finally
    print("finally")
end
""", "finally with throw")

test_expect_result(r"""try-catch: try
finally
""", r"""
try
    print("try-catch: try")
catch
    print("try-catch: catch")
finally
    print("finally")
end
""", "finally no throw")

test_expect_result(r"""try-catch: try
no error
finally
""", r"""
try
    print("try-catch: try")
catch
    print("try-catch: catch")
else
    print("no error")
finally
    print("finally")
end
""", "finally no throw")

test_expect_result(r"""inner
outer
""", r"""
try
    try
        throw
    catch
        print("inner")
        throw
    end
catch
    print("outer")
end""", "nested try")
