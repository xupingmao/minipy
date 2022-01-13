
a = "test"

b = a in ["test", "test2", "test3"]
assert b is True, 'failed: a in ("test", "test2", "test3")'

def func():
	pass

def func2():
	pass

a = func
b = a in [func, func2]

assert b is True, 'failed: a in (func, func2)'