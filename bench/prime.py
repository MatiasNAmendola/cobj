maxnum = 100000
i = 2

def is_prime(n):
    j = 2
    while j * j < n + 1:
        if n % j == 0:
            return False
        j += 1
    return True

while i <= maxnum:
    if is_prime(i):
        print(i)
    i += 1
