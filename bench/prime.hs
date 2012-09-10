minus (x:xs) (y:ys) = case (compare x y) of 
           LT -> x : minus  xs  (y:ys)
           EQ ->     minus  xs     ys 
           GT ->     minus (x:xs)  ys
minus  xs     _     = xs

primesToQ m = 2 : sieve [3,5..m] where
        sieve []     = []
        sieve (p:xs) = p : sieve (xs `minus` [p*p, p*p+2*p..m])

main :: IO ()
main = do
    mapM_ print (primesToQ 100000)
