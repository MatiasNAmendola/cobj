function fibo(n) {
    if (n == 0)
        return 0
    if (n == 1)
        return 1
    return fibo(n - 1) + fibo(n - 2)
}
i = 0
while (i < 30) {
    console.log(fibo(i))
    i += 1
}
