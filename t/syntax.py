#!/usr/bin/env python
from TAP.Simple import *

plan("no_plan")

test_expect_result("", """func empty() {}""", "allow empty function");
