from io import StringIO
import sys

_blacklist = [
    "FindNextFile",
    "dwFileAttributes",
    "cFileName",
    "FindFirstFile"
]

_convert_dict = {
    "Tm_list": "TmList",
    "Tm_string": "TmString",
    "Tm_dict": "TmDict",
    "Tm_vm":"TmVm",
    "Tm_v_m": "TmVm",
    "Tm_function": "TmFunction",
    "Tm_value": "TmValue",
    "Tm_module": "TmModule",
    "Tm_frame": "TmFrame",
    "Tm_data": "TmData",
    "Tm_dict_iterator": "TmDictIterator",
    "Parser_ctx":"ParserCtx",
    "Ast_node": "AstNode",
    "Asm_context" :"AsmContext",
    "Encode_ctx": "EncodeCtx",
    "Dict_node": "DictNode",
    "Tm_list_iterator": "TmListIterator",
}

def do_name(line, i):
    newname = ''
    oldname = ''
    while i < len(line):
        c = line[i]
        if c.isalnum() or c == '_':
            oldname += c
            if c.isupper():
                newname += "_" + c.lower()
            else:
                newname += c
        else:
            break
        i+=1
    if oldname in _blacklist:
        return oldname, i 
    if oldname in _convert_dict:
        return _convert_dict[oldname], i
    if oldname[0].isupper():
        return oldname, i
    return newname, i

def do_skip_str(line, i, end):
    str = ''
    while i < len(line):
        c = line[i]
        str += c
        if c == end:
            i += 1
            break
        if c == '\\':
            str += line[i+1]
            i += 2
        else:
            i += 1
    return str, i

def convert (content):
    buf = StringIO()
    i = 0
    while i < len(content):
        c = content[i]
        if c == '"' or c == "'":
            str, i = do_skip_str(content, i, c)
            buf.write(str)
        elif c.isalpha():
            newname, i = do_name(content, i)
            buf.write(newname)
        else:
            buf.write(c)
            i+=1
    buf.seek(0)
    return buf.read()

def save_file(name, content):
    fp = open(name, "w+")
    fp.write(content)
    fp.close()


def do_opt(opt):
    if opt == "-test":
        result = convert("this is a test CamelCase camelCase\nHello")
        print(result)
    else:
        name = sys.argv[1]
        content = open(name).read()
        result = convert(content)
        if content != result:
            save_file(name + ".bak", content)
            save_file(name, result)

if __name__ == '__main__':
    if len(sys.argv) == 2:
        opt = sys.argv[1]
        do_opt(opt)
    else:
        print ("usage %s filename" % sys.argv[0])