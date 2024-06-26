# -*- coding:utf-8 -*-
'''
Author: xupingmao
email: 578749341@qq.com
Date: 2023-12-07 22:03:29
LastEditors: xupingmao
LastEditTime: 2024-05-25 07:27:33
FilePath: /minipy/test/benchmark/benchmark_main.py
Description: 描述
'''
import time
import os
import sys
import argparse

def timeit(func, *args):
    t1 = time.time()
    ret = func(*args)
    t2 = time.time()
    print("run time is ", t2-t1)

def exec_bench(executable, fpath):
	cmd = "%s \"%s\"" % (executable, fpath)
	# print("CMD:", cmd)
	os.system(cmd)

def exists_tinypy():
	ret = os.system("build/tinypy")
	return ret == 0

def main():
	parser = argparse.ArgumentParser("Minipy基准测试")
	parser.add_argument("--micropython", action = "store_true")
	parser.add_argument("--target", default="")
	args = parser.parse_args()

	dirname = "./test/benchmark/cases"
	dirname = os.path.abspath(dirname)

	minipy_executable = "./minipy"
	if os.name == "nt":
		minipy_executable = "minipy.exe"
	
	for fname in sorted(os.listdir(dirname)):
		if args.target != "" and fname != args.target:
			continue
		
		fpath = os.path.join(dirname, fname)
		print("File:", fname)
		print("-" * 60)
		print(">>> Run with Python3")
		exec_bench("python3", fpath)
		

		print("-" * 60)
		print(">>> Run with minipy")
		exec_bench(minipy_executable, fpath)

		# print("-" * 60)
		# print(">>> Run with minipy-mp2c")
		# os.system("./minipyc %r -o %r.out && ./%r.out" % fpath)

		if exists_tinypy():
			print("-" * 60)
			print(">>> Run with tinypy")
			exec_bench("build/tinypy", fpath)

		if args.micropython:
			print("-" * 60)
			print(">>> Run with MicroPython")
			os.system("./build/micropython %r" % fpath)

		print("\n\n")


if __name__ == '__main__':
	main()