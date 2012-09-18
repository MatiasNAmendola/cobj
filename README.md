This is CObj, an general-purpose program language.

Compile/Install
===============
    # install bison/re2c first
    $ make  
    $ make install

Examples
========

    maxnum = 10
    i = 2

    func is_prime(n)
        local j = 2
        while j * j < n + 1
            if n % j == 0
                return False
            end
            j += 1
        end
        return True
    end

    while i <= maxnum
        if is_prime(i)
            print(i)
        end
        i += 1
    end

See more examples under `examples/` folder.
