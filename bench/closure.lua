base = 10
newIncrer = function (n)
  return function(i)
    n = n + i
    return function(j)
      n = n + j + base
      return n
    end
  end
end

f = newIncrer(10)
f = f(1)
print(f(1))
