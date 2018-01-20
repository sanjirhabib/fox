class fib {
	public static void main(String args[]){
		System.out.println(fib(42));
	}
	public static int fib(int num){
		if(num<2) return num;
		return fib(num-1)+fib(num-2);
	}
}
