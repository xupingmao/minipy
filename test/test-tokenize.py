from tokenize import *

def tk_types(r):
    rr = []
    for tk in r:
        rr.append(tk.type)
    return rr
    
def tk_vals(r):
    rr = []
    for tk in r:
        rr.append(tk.val)
    return rr
    
def tk_test(string, tokens):
    print("test", string)
    r = tokenize(string)
    for i in range(len(r)):
        tk = r[i]
        assert tk.type == tokens[i*2]
        assert tk.val  == tokens[i*2+1]
    
def run():
    tk_test("def man() pass", [
        "def", "def",
        "name","man",
        "(", "(",
        ")", ")",
        "pass", "pass"])
    tk_test("a = 10 + 2", [
        "name", "a",
        "=", "=",
        "number", 10,
        "+", "+",
        "number", 2
    ])
    
run()