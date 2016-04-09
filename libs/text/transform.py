from tokenize import *
from parse import *
from boot import *
from tmcode import *

def hasnext(list, i):
    return i < len(list)
def getnext(list, i):
    if i < len(list)-1:
        return list[i+1]
    return None

def printindents(i):
    printf(' '*i)
    
def stringAppendItem(code, item, sep=","):
    if code == "" or code==None:
        return item
    else:
        return code + sep + item

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
def transform_tuple(itemList):
    if itemList == None: return ""
    code = None
    for item in itemList:
        code = stringAppendItem(code, transform_item(item))
    return code
    
def get_printable_str(s):
    if '\\' in s:
        s = s.replace('\\', '\\\\')
    if '\n' in s:
        s = s.replace('\n', '\\n')
    if '\t' in s:
        s = s.replace('\t', '\\t')
    if '\b' in s:
        s = s.replace('\b', '\\b')
    if '\r' in s:
        s = s.replace('\r', '\\r')
    if '"' in s:
        s = s.replace('"', '\\"')
    if '\0' in s:
        newstr=''
        for c in s:
            if c == '\0':
                newstr += '\\0'
            else:
                newstr += c
        return '"' + newstr + '"'
    return '"' + str(s) + '"'
    
def transformDict(dictItem):
    if dictItem == None:
        return ""
    for node in dictItem:
        if dictStr == None:
            dictStr = transform_item(node.first)
            dictStr += ":" + transform_item(node.second)
        else:
            dictStr += "," + transform_item(node.first)
            dictStr += ":" + transform_item(node.second)
    return dictStr
    
def transformWhile(whileItem):
    cond = transform_item(whileItem.first)
    body = transform_item(whileItem.second)
    return "while(" + cond + "){\n" + body + "\n}\n"
    
    
def transformArgs(args):
    if args==None: return ""
    if istype(args, "list"):
        code = ""
        for arg in args:
            code = stringAppendItem(code, transform_item(arg))
        return code
    return transform_item(args)

def transform_item(item, gap = 0):
    if item == None:return ""
    if istype(item, "list"):
        code = ""
        for node in item:
            code = stringAppendItem(code, transform_item(node), "\n")
        return code
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
    elif item.type == "dict":
        return transformDict(item)
    elif item.type == ',':
        return transform_item(item.first) + ', ' + transform_item(item.second)
    elif item.type == 'None':
        return 'undefined'
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
    elif item.type == "not":
        return "not " + transform_item(item.first)
    elif item.type == "neg":
        return "-" + transform_item(item.first)
    elif item.type == 'get':
        return transform_item(item.first) + '[' + transform_item(item.second) + ']'
    elif item.type == 'attr':
        return transform_item(item.first) + '.' + transform_item(item.second)
    elif item.type == 'call':
        return transform_item(item.first) + '(' + transformArgs(item.second) + ')'
    elif item.type == 'if':
        return transform_if("if", item, gap)
    elif item.type == 'for':
        return transform_if('for', item, gap)
    elif item.type == "while":
        return transformWhile(item)
    elif item.type == "break":
        return "break;"
    elif item.type == "continue":
        return "continue"
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
    