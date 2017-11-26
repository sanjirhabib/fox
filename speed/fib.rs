fn main() {
	print!("{} ", fib(40));
}
fn fib(number: u32) -> u32 {
	if number == 0 {
		0
	}
    else if number == 1 {
		1
	}
    else {
		fib(number - 1) + fib(number - 2)
	}
}
