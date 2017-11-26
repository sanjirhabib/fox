function fib(i)
	if i<2 then return i end
	return fib(i-1)+fib(i-2)
end
print(fib(32))
