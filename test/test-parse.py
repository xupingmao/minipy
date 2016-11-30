from parse import *

def assert_name(tk, val):
    assert tk.type == "name"
    assert tk.val  == val
    
def assert_str(tk, val):
    assert tk.type == "string"
    assert tk.val  == val

def assert_num(tk, val):
    assert tk.type == "number"
    assert tk.val  == val
    
def test_import():
    code = "import abc"
    astlist = parse(code)
    ast = astlist[0]
    assert ast.type == "import"
    assert_name(ast.first, "abc")
    
def test_from():
    # only support `from name import *`
    code = "from a import *"
    ast = parse(code)[0]
    assert ast.type == "from"
    assert_name(ast.first, "a")
    assert_str(ast.second, "*") 
    

def test_assignment():
    code = "a = 10"
    ast = parse(code)[0]
    assert ast.type == "="
    assert_name(ast.first, "a")
    assert_num(ast.second, 10)
    
    code = '''a = "abc" '''
    ast = parse(code)[0]
    assert ast.type == "="
    assert_name(ast.first, "a")
    assert_str(ast.second, "abc")
    
def run_test(testlist):
    for test in testlist:
        print(test)
        test()
        
run_test([
    test_import,
    test_from,
    test_assignment,
])