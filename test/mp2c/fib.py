

def fib(n):
	if n <= 0:
		raise "n<=0, n=%s" % n
	if n == 1 or n == 2:
		return 1
	return fib(n-1) + fib(n-2)

print("fib(30)=" + str(fib(30)))