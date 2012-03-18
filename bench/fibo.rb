def fibo(n)
    return 0 if n == 0
    return 1 if n == 1
    return fibo(n - 1) + fibo(n - 2)
end

i = 0
while i < 30
    p fibo(i)
    i += 1
end
