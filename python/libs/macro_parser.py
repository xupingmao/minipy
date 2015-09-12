from tokenize import *

# whitespace before
_ws_both = ['and', 'or', 'in', 'not']
# whitespace after
_ws_after = ['function', 'if', 'while', 'return', 'def']


# the element in list must object.
def list_to_chain(list):
    size = len(list)
    i = 0
    for i in range(0, size - 1):
        list[i].next = list[i+1]
    list[size-1].next = None
    return list

def parse_file(fname):
    customize_symbols(['function', 'var', 'end', 'append'], '-=[];,./!%*()+{}:<>@^$&', [
    '-','+','*','**','/','%','<<','>>',
    '-=','+=','*=','/=','=','==','!=','<','>',
    '<=','>=','[',']','{','}','(',')','.',':',',',';','&',
    '|','!','@','^','$'
    ]);
    disable_tk_indent()
    list = tokenize(load(fname))
    list = list_to_chain(list)
    if len(list) == 0:return
    item = list[0]
    #while item != None:
        #print(item.val)
        #item = item.next
    parse_macro(item)
    
_func_dict = None
_var_dict = None
_init = 0
_cur = None
def parse__init():
    global _func_dict
    global _var_dict
    _func_dict = {}
    _var_dict = {}

def parse_error(func, msg):
    print('at ' + func)
    print(msg)
    exit(1)
    
def def_func_params(func_name, func, param):
    params = []
    params.append(param.val)
    predict = param.next
    while predict.val == ',':
        param = predict.next
        params.append(param.val)
        predict = param.next
    if predict.val != ')':
        parse_error('<def_func_params>:'+func_name, 'need ")"')
    func.params = params
    return predict.next
    
def def_func(item):
    global _cur
    func = obj_new()
    name = item.next
    func.type = 'func'
    func.name = name.val
    _func_dict[func.name] = func
    left_paren = name.next
    predict = left_paren.next
    if predict.type != ')':
        func.params = None
        rest = def_func_params(name.val, func, predict)
    else:
        rest = predict.next
    if rest.val != '=':
        parse_error('<def_func>:'+name.val, 'need "="')
    rest = rest.next
    body = rest
    while rest != None and rest.val != 'end':
        if rest.type == '@':
            rest = rest.next
            var = rest.val
            idx = func.params.index(var)
            rest.val = 'number'
            rest.val = idx
        rest = rest.next
    _cur = rest
    func.body = body
    return func
    
def lookup_var(name, args = None):
    if name == None:
        parse_error('<lookup_var>', 'invalid name reference')
    global _cur
    _cur = name
    if args != None:
        return args[int(name.val)]
    return _var_dict[name.val]
    
def mp_assert_type(tk, tar, func):
    if tk.type != tar:
        parse_error(msg, 'assert fail')
        
        
def call_func(name):
    if name == None:
        parse_error('<call_func>', 'invalid name reference')
    global _cur
    func = _func_dict[name.val]
    #print_func(func)
    left_paren = name.next
    mp_assert_type(left_paren, '(', '<call_func>')
    predict = left_paren.next
    args = []
    if predict == None:
        parse_error('<call_func>', 'invalid token after "("')
    while predict.val != ')':
        arg = get_value(predict)
        args.append(arg)
        predict = _cur.next
        if predict.val == ',':
            predict = predict.next
    mp_assert_type(predict, ')', '<call_func>')
    temp_cur = predict
    s = ''
    item = func.body
    while item != None and item.type != 'end':
        s += get_value(item, args)
        item = _cur.next
    _cur = temp_cur
    rs = obj_new()
    rs.type = 'string'
    rs.val = s
    return rs
            
def get_value(item, args = None):
    global _cur
    type = item.type
    if type == '@':
        return lookup_var(item.next, args)
    elif type == '[':
        return assemble_arr(item)
    elif type == 'string':
        _cur = item
        return '"' + item.val + '"'
    elif type == '&':
        predict = item.next
        if predict.type == 'string':
            _cur = predict
            return predict.val
        else:
            _cur = item
            return item.val
    else:
        _cur = item
        if item.val == '^':
            return '    '
        if item.val == 'nl':
            return '\n'
        if item.val in _ws_both:
            return ' ' + str(item.val) + ' '
        elif item.val in _ws_after:
            return str(item.val) + ' '
        return str(item.val)
def skip_comment(item):
    _cur = item
    while item != None and item.val != 'nl':
        item = item.next
    _cur = item
def assemble_arr(item):
    global _cur
    item = item.next
    s = '['
    while item != None and item.type != ']':
        s += get_value(item)
        item = _cur.next
    _cur = item
    return s + ']'
def parse_stmt(item, eval_flag = 1):
    global _cur
    if item == None:return None
    val = item.val
    if val == 'function':
        return def_func(item)
    elif val == 'var':
        return def_var(item)
    elif val == '@':
        return lookup_var(item.next)
    elif val == '$' and eval_flag:
        return call_func(item.next)
    elif val == '#':
        skip_comment(item)
        return parse_stmt(parse_cur)
    elif val == 'nl':
        return parse_stmt(item.next, eval_flag)
    elif val == None:
        return None
    else:
        _cur = item
        return item

def def_var(item):
    name = item.next
    eq = name.next
    value = get_value(eq.next)
    _var_dict[name.val] = value
    return None
    
def print_func(func):
    print('function', func.name)
    print(func.params)
    item = func.body
    while item != None and item.type != 'end':
        if item.type == 'nl':
            print("")
        elif item.type == '^':
            printf('    ')
        elif item.type == 'string':
            printf('"')
            printf(item.val)
            printf('"')
        else:
            if item.val in _ws_both:
                printf(' ')
                printf(item.val)
                printf(' ')
            elif item.val in _ws_after:
                printf(item.val)
                printf(' ')
            else:
                printf(item.val)

        item = item.next
def parse_print(item):
    if item == None:
        pass
        #print(None)
    elif item.type == 'func':
        #print_func(item)
        pass
    elif item.type == 'nl':
        # print("")
        pass
    elif item.type == 'string':
        print(item.val)
def parse_macro(item):
    global _init
    if not _init:
        parse__init()
        _init = 1
    global _cur
    _cur = item
    while _cur != None:
        x = parse_stmt(_cur)
        parse_print(x)
        #print("")
        if _cur == None:
            break
        _cur = _cur.next
        
if __name__ == '__main__':
    parse_file('grammar.txt');