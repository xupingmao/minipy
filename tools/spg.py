

# simple parser generator

class SpgType(object):
    """docstring for SpgType"""
    def __init__(self, type, value):
        super(SpgType, self).__init__()
        self.type = type
        self.value = value
        

_none_terminal_dict = None

def spg_init():
    global _none_terminal_dict
    _none_terminal_dict = {}

def spg_repeat(*st_list):
    list = []
    for item in st_list:
        list.append(spg_wrapper(item))
    return SpgType("repeat", list)

def spg_list(*st_list):
    list = []
    for item in st_list:
        list.append(spg_wrapper(item))
    return SpgType("list", list)

def spg_wrapper(value):
    if isinstance(value, str):
        return SpgType("id", value)
    return value

def spg_or(*or_list):
    list = []
    for item in or_list:
        list.append(spg_wrapper(item))
    return SpgType("or", list)

def spg_def(name, st):
    _none_terminal_dict[name] = st

def spg_find(name):
    return _none_terminal_dict.get(name)

def is_terminal(name):
    return name not in _none_terminal_dict

def find_terminal(expr):
    if expr.type == "id":
        name = expr.value
        expr1 = spg_find(name)
        if expr1 is None:
            return "expect(%s)" % name
        else:
            find_terminal(expr1)
    elif expr.type == "repeat":
        return find_terminal(expr.value[0])

def spg_gen1(item):
    body = []
    if item.type == "id":
        name = item.value
        expr = spg_find(name)
        if expr is None:
            body.append("expect('%s')" % name)
        else:
            body.append("%s(ctx)" % name)
    elif item.type == "repeat":
        # first = item.value[0]
        # del item.value[0]
        # check = find_terminal(first)
        body.append("while (true) {")
        # body.append("if (! %s ) break;" % check)
        # body.append(" if (! %s) break; " % spg_gen0(item.value))
        for item0 in item.value:
            body.append(spg_gen1(item0))
            body.append("if (_spg_error) break;")
        body.append("}")
    elif item.type == "or":
        orlist = []
        orlist.append("int current_pos = save_pos(ctx)")
        cond_cnt = 0
        for item0 in item.value:
            if cond_cnt == 0:
                orlist.append("if (%s) {} " % spg_gen1(item0))
                cond_cnt += 1
            else:
                orlist.append("else { ")
                orlist.append("restore_pos(ctx, current_pos)");
                orlist.append("if (%s) {}" % spg_gen1(item0))
                cond_cnt += 1
        for i in range(cond_cnt-1):
            orlist.append("}")
        return "\n".join(orlist)
    elif item.type == "list":
        for item0 in item.value:
            body.append(spg_gen1(item0))
        return "\n".join(body)
    return "\n".join(body)

def spg_gen0(st):
    if isinstance(st, list):
        slist = []
        for item in st:
            slist.append(spg_gen1(item))
        return "\n".join(slist)
    else:
        return spg_gen1(st)


def spg_gen():
    for name in _none_terminal_dict:
        st = _none_terminal_dict[name]
        body = spg_gen0(st)
        print("%s \n%s \n end \n\n" % (name, body))

# eg.
# expr = { item + | - item }
# item = number * | / number

spg_init()
expr = spg_repeat('item', spg_or('+', '-'), 'item')

item = spg_list('number', spg_repeat(spg_or('*', '/'), 'number'))

spg_def('item', item)
spg_def('expr', expr)

value = spg_gen()
print(value)

