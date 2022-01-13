
print("a = 10")
a = 10
assert a == 10, "failed: a = 10"

print("a, b = 1, 2")
a, b = 1, 2

assert a == 1, "failed: a, b = 1, 2"
assert b == 2, "failed: a, b = 1, 2"

class T:
	pass


t = T()
t.a, t.b = 2, 3

assert t.a == 2, "failed: t.a, t.b = 2, 3"
assert t.b == 3, "failed: t.a, t.b = 2, 3"