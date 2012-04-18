package main

import (
    "fmt"
)

func IsPrime(n int) bool {
    for j := 2; j * j < n + 1; j++ {
        if n % j == 0 {
            return false
        }
    }
    return true
}

func main() {
    for i := 2; i < 100000; i++ {
        if IsPrime(i) {
            fmt.Printf("%d\n", i)
        }
    }
}

