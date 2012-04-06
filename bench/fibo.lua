function fibo(n)
  if n == 0 then
    return 0
  end
  if n == 1 then
    return 1
  end
  return fibo(n - 1) + fibo(n - 2)
end

i = 0
while i < 30 do
  print(fibo(i))
  i = i + 1
end
