from tokenize import *
from parse import *
from boot import *
from test import *
from tmcode import *

def hasnext(list, i):
    return i < len(list)
def getnext(list, i):
    if i < len(list)-1:
        return list[i+1]
    return None

def printindents(i):
    printf(' '*i)

# def printstr(s):
#     if '"' not in s:
#        quote = '"'
#     elif "'" not in s:
#        quote = "'"
#     else:
#        quote = "'''"
#     printf(quote)
#     for i in s:
#         if i == '\n':printf('\\n')
#         elif i == '\b':printf('\\b')
#         elif i == '\r':printf('\\r')
#         elif i == '\t':printf('\\t')
#         elif i == '\0':printf('\\0')
#         elif i == '\\':printf('\\\\')
#         else:printf(i)
#     printf(quote)

_ws_both = ['import', 'as']
_ws_after = ['for', 'if', 'elif', 'while', 'def', 'return', 
    'from', 'class', 'global', 'not', 'except', ',']

def transform_list(list, gap = 0):
    s = ""
    for item in list:
        s += gap * ' ' + transform_item(item, gap) + "\n"
    return s

_op_list = ['+', '-', '*', '/', '=', '+=', '-=', '*=',
     '/=', '==', '>=', '<=', '!=', '>', '<', 'and', 'or', 'in', 'notin']

_ws_both += _op_list
def transform_args(args):
    s = ""
    i = 0
    while i < len(args):
        s += transform_item(args[i])
        i += 1
        if i < len(args):
            s += ', '
    return s
def transform_tuple(item):
    if item == None: return ""
    if item.type == ',':
        return transform_item(item.first) + ', ' + transform_item(item.second)
    return transform_item(item)
def transform_item(item, gap = 0):
    if item == None:return ""
    if item.type == 'name':
        return item.val
    elif item.type == 'number':
        return str(item.val)
    elif item.type == 'string':
        return get_printable_str(item.val)
    elif item.type == 'arg':
        return item.first.val
    elif item.type == 'narg':
        return '*' + item.first.val
    elif item.type == 'list':
        return '[' + transform_tuple(item.first) + ']'
    elif item.type == ',':
        return transform_item(item.first) + ', ' + transform_item(item.second)
    elif item.type == 'None':
        return 'None'
    elif item.type == 'return':
        return 'return ' + transform_item(item.first)
    elif item.type =='def':
        name = item.first
        args = item.second
        body = item.third
        head = '\nfunction ' + name.val + '(' + transform_args(args) + ')'
        body = transform_list(body, gap + 2)
        return head + ' {\n' + body + '}\n'
    elif item.type in _op_list:
        return transform_item(item.first) +' ' + item.type + ' ' \
            + transform_item(item.second)
    elif item.type == 'get':
        return transform_item(item.first) + '[' + transform_item(item.second) + ']'
    elif item.type == '.':
        return transform_item(item.first) + '.' + transform_item(item.second)
    elif item.type == '$':
        return transform_item(item.first) + '(' + transform_item(item.second) + ')'
    elif item.type == 'if':
        return transform_if("if", item, gap)
    elif item.type == 'for':
        return transform_if('for', item, gap)
    elif item.type == 'elif':
        return transform_if("} else if", item, gap)
    elif item.type == 'from':
        # replPrint (item)
        return 'from ' + transform_item(item.first) + " import " + transform_item(item.second)
    elif item.type == 'import':
        return 'import ' + transform_item(item.first)
    return ""


def transform_if(prefix, item, gap):
    head = prefix + "("
    cond = transform_item(item.first)
    body = transform_list(item.second, gap+2)
    if not hasattr(item, 'third'):
        return head + cond + ') {\n' + body + '%s}'.format(gap * ' ')
    if item.third:
        if istype(item.third, "list"):
            rest = transform_list(item.third, gap+2)
        else:
            if item.third.type == 'if':
                item.third.type = 'elif'
                rest = transform_item(item.third, gap)
                return head + "%s) {\n%s".format(cond, body) + ' ' * gap + rest
            rest = transform_item(item.third, gap+2)
        return head + cond + "){\n" \
            + body + \
            "%s} else {\n".format(' ' * gap) + \
            rest + "%s}".format(' ' * gap)
    else:
        return head + cond + ") {\n" \
            + body + \
            "%s}".format(gap * ' ') 

def transform2(fname):
    r = parsefile(fname)
    r = transform_list(r)
    printf(r)

def transform(fname):
    s = load(fname)
    tokenList = tokenize(s)
    indents = 0
    idx = 0
    lastisnl = False
    needspace = False
    while hasnext(tokenList, idx):
        i = tokenList[idx]
        next = getnext(tokenList, idx)
        idx += 1
        if i.type in _ws_after:
            printf("%s ", i.val)
        elif i.type in _ws_both:
            printf(" %s ", i.val)
        elif i.type == 'nl':
            printf('\n')
            if next == None:
                pass
            elif next.type == 'indent':
                printf(' '* (indents + 4))
            elif next.type == 'dedent':
                #printf(' '*(indents - 4))
                pass
            else:
                printf(' '* indents)
        elif i.type == 'indent':
            indents += 4
        elif i.type == 'dedent':
            indents -= 4
            if next == None:
                pass
            elif next.type != 'dedent':
                printf(' ' * indents)
        elif i.type == 'string':
            printf(get_printable_str(i.val))
        elif i.type == 'notin':
            printf(' not in ')
        else:
            printf(i.val)
def print_usage():
    printf("usage: %s {file}\n", ARGV[0])
    printf("       %s -h {file}\n", ARGV[0])
    exit()

def main(ARGV):
    from libs.tools.repl import replPrint
    if len(ARGV) < 2:
        print_usage()
    if len(ARGV) == 2:
        transform(ARGV[1])
    elif len(ARGV) == 3:
        if ARGV[1] == '-h':
            transform2(ARGV[2])
        else:
            print_usage()
if __name__ == '__main__':
    main(ARGV)
    