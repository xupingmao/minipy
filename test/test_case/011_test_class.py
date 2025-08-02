
class MyClass:

    def __init__(self):
        self.data = []

    def add(self, item):
        self.data.append(item)

    def __len__(self):
        return len(self.data)
    
    def __getattr__(self, attr, default = None):
        if attr == "first":
            return self.data[0]
        if attr == "second":
            return self.data[1]
        return default
    
    def __str__(self):
        return "<MyClass>"

c = MyClass()
c.add(10)
c.add(20)

assert len(c) == 2
assert c.first == 10
assert c.second == 20
assert str(c) == "<MyClass>"
