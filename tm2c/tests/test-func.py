# test file for tm2c.py

print("hello,world")

#@static
def this_is_a_func():
    return "hello"
    
    
def test_arg(a,b):
    return a + b

_msg = this_is_a_func()

print (_msg)