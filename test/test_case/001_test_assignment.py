# -*- coding:utf-8 -*-
'''
Author: xupingmao
email: 578749341@qq.com
Date: 2022-02-12 11:48:21
LastEditors: xupingmao
LastEditTime: 2024-06-02 02:30:12
FilePath: /minipy/test/test_case/001_test_assignment.py
Description: 描述
'''

print("a = 10")
a = 10
assert a == 10, "failed: a = 10"

print("a, b = 1, 2")
a, b = 1, 2

assert a == 1, "failed: a, b = 1, 2"
assert b == 2, "failed: a, b = 1, 2"

class T:
	def __init__(self, a=0, b=0):
		self.a = a
		self.b = b


t = T()
t.a, t.b = 2, 3

print(t)
print(t.a)
print(t.b)

assert t.a == 2, "failed: t.a, t.b = 2, 3"
assert t.b == 3, "failed: t.a, t.b = 2, 3"