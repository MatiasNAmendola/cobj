maxnum = 100000
i = 2

function is_prime(n)
    j = 2
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
