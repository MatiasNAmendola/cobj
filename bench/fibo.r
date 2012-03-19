REBOL [
    Title: "Fibo"
]

fibo: func [n] [
    switch/default n [
        0   [0]
        1   [1]
    ] [ 
        (fibo n - 1) + (fibo n - 2)
    ]
]

i: 0
while [i < 30] [
    print [fibo i]
    i: i + 1
]
