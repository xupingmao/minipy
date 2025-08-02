
s = set()
s.add(1)
s.add(2)
s.add(1)

assert len(s) == 2, "set test 1"

s = set([1,2])
s.add(2)

assert len(s) == 2, "set test 2"
