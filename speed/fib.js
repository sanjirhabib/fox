function fib(i){
	if(i<2) return i
	return fib(i-1)+fib(i-2)
}
console.log(fib(32))
