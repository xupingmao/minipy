from mp_parse import *

def assert_name(tk, val):
    assert tk.type == "name"
    assert tk.val  == val
    
def assert_str(tk, val):
    assert tk.type == "string"
    assert tk.val  == val

def assert_num(tk, val):
    assert tk.type == "number"
    assert tk.val  == val
    
def assert_list(tk, val=None):
    assert istype(tk, "list")
    if val:
        assert len(tk) == val
    
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
    
def test_list():
    code = "a = [1,2,3]"
    ast = parse(code)[0].second
    assert ast.type == "list"
    nodes = ast.first
    assert_list(nodes, 3)
    assert_num(nodes[0], 1)
    assert_num(nodes[2], 3)
    
    
def test_assignment():
    code = "a = 10"
    ast = parse(code)[0]
    assert ast.type == "="
    assert_name(ast.first, "a")
    assert_num(ast.second, 10)
    
    code = '''
a = "abc"
'''
    ast = parse(code)[0]
    assert ast.type == "="
    assert_name(ast.first, "a")
    assert_str(ast.second, "abc")
    
def test_if():
    code = "if x: print(10)"
    ast = parse(code)[0]
    assert ast.type == "if"
    assert_name(ast.first, "x")
    assert_list(ast.second, 1)
    
def test_if2():
    code = '''
if x:
    print(10)
else:
    print(0)
'''
    ast = parse(code)[0]
    assert ast.type == "if"
    assert_name(ast.first, "x")
    assert_list(ast.second, 1)
    elsepart = ast.third
    assert_list(elsepart, 1)
    
def test_if3():
    code = '''
if x:
    print(10)
elif x > 10:
    print(20)
else:
    print(0)
'''
    ast = parse(code)[0]
    assert ast.type == "if"
    assert_name(ast.first, "x")
    assert_list(ast.second, 1)
    
    
def run_test(testlist):
    for test in testlist:
        print(test)
        test()
        
run_test([
    test_list,
    test_import,
    test_from,
    test_assignment,
    test_if,
    test_if2,
    test_if3,
])