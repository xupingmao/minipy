import time
import os

def timeit(func, *args):
    t1 = time.time()
    ret = func(*args)
    t2 = time.time()
    print("run time is ", t2-t1)


def main():
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

		print("-" * 60)
		print(">>> Run with tinypy")
		os.system("./build/tinypy %r" % fpath)

		print("\n\n")


if __name__ == '__main__':
	main()