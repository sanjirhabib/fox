using System;
public class Fib {
	public static int fib (int n) {
		if (n < 2) return n;
		return fib(n-2)+fib(n-1);
	}
	public static int Main (string[] args) {
		Console.WriteLine(fib(32));
		return 0;
	}
}
