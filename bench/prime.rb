maxnum = 100000
i = 2

def is_prime(n)
    j = 2
    while j * j < n + 1
        if n % j == 0
            return false
        end
        j += 1
    end
    return true
end


while i <= maxnum
    p i if is_prime(i)
    i += 1
end

