maxnum = 100000
i = 2

function is_prime(n) -- n is local implicitly
    local j = 2 -- make it local, performance critical
    while j * j < n + 1 do
        if n % j == 0 then
            return false
        end
        j = j + 1
    end
    return true
end

while i <= maxnum do
    if is_prime(i) then
        print(i)
    end
    i = i + 1
end
