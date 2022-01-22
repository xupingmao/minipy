
import debug
import random

def rand_str(length):
	v = ""
	a = ord('A')
	b = ord('Z')
	for i in range(length):
		v += chr(random.randint(a, b))
	return v

def main(n):
	print("n=%d" % n)
	d = dict()
	for i in range(n):
		key = rand_str(5)
		d[key] = 1
	debug.print_dict_info(d)

main(5)
print("-" * 50)

main(10)
print("-" * 50)

main(15)
print("-" * 50)

main(20)
print("-" * 50)

main(30)
print("-" * 50)