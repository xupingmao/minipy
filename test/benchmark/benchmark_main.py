import time
import os
import sys
import argparse

def timeit(func, *args):
    t1 = time.time()
    ret = func(*args)
    t2 = time.time()
    print("run time is ", t2-t1)


def main():
	parser = argparse.ArgumentParser("Minipy基准测试")
	parser.add_argument("--micropython", action = "store_true")
	args = parser.parse_args()

	dirname = "./test/benchmark/cases"
	for fname in sorted(os.listdir(dirname)):
		fpath = os.path.join(dirname, fname)
		print("File:", fname)
		print("-" * 60)
		print(">>> Run with Python3")
		os.system("python3 %r" % fpath)

		print("-" * 60)
		print(">>> Run with minipy")
		os.system("./minipy %r" % fpath)

		# print("-" * 60)
		# print(">>> Run with minipy-mp2c")
		# os.system("./minipyc %r -o %r.out && ./%r.out" % fpath)

		print("-" * 60)
		print(">>> Run with tinypy")
		os.system("./build/tinypy %r" % fpath)

		if args.micropython:
			print("-" * 60)
			print(">>> Run with MicroPython")
			os.system("./build/micropython %r" % fpath)

		print("\n\n")


if __name__ == '__main__':
	main()