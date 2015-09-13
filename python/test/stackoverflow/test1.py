
def foo(a, b, c, d):
    if d > 0:
        foo(a, b, c, d-1)
       
foo(1, 1, 1, 1024) 