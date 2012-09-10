var maxnum = 100000
var i = 2

function is_prime(n) {
    var j = 2
    while (j * j < n + 1) {
        if (n % j == 0)
            return false
        j += 1
    }
    return true
}


while (i <= maxnum) {
    if (is_prime(i)) 
        console.log(i)
    i += 1
}
