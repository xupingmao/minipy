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
    
def tk_test(string, types, vals):
    r = tokenize(string)
    assert tk_types(r) == types
    assert tk_vals(r) == vals
    
def run():
    tk_test("def man() pass", ["def", "name", "(", ")", "pass"], ["def", "man", "(", ")", "pass"])
    
run()