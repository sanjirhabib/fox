package main
import "fmt"
func fib(n uint) uint {
    if n == 0 {
        return 0
    } else if n == 1 {
		return 1
	} else {
		return fib(n-1) + fib(n-2)
	}
}
func main() {
	n := uint(42)
	fmt.Println(fib(n))
}
