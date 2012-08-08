Visibility Rules
-----------

CO is a lexically scoped language. The scope of a local variable begins at the first statement after its declaration and lasts until the last non-void statement of the innermost block that includes the declaration. Consider the following example:::

    x = 10              # global variable
    i = 0
    while i < 10        # new block
        local x = x     # new 'x', with value 10
        x += 1
        while i > 5     # new block
            local x     # another 'x'
            x += 1
            print(x)    # 12
        end
        print(x)        # 11
        i += 1
    end

Because of the lexical scoping rules, local variables can be freely accessed by functions defined inside their scope. A local variable used by an inner function is called an upvalue, or external local variable, inside the inner function.

Notice that each execulation of a local statement defines new local variables. Consider the following example:::

    a = {}
    local x = 20
    local i = 0
    while i < 10
        local y = 0
        a[i] = func () y = y + 1; return x + y end
        i += 1
    end

The loop creates ten closures (that is, ten instances of the anonymous function). Each of these closures uses a different y variable, while all of them share the same x.
