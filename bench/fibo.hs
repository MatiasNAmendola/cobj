fibos :: [Int]
fibos = 0:1:zipWith (+) fibos (tail fibos)

main :: IO ()
main = do
    mapM_ print (slice 0 29 fibos)
    where
        slice start end = take (end - start + 1) . drop start
