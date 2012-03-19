REBOL [
    Title: "Prime"
]

maxnum: 100000
i: 2

is_prime: func [n] [
    j: 2
    r: true
    while [(j * j) < (n + 1)] [
        if ((modulo n j) == 0) [
            r: false
            break
        ]
        j: j + 1
    ]
    r
]

while [i <= maxnum] [
   if (is_prime i) [
        print i
   ]
   i: i + 1
]
