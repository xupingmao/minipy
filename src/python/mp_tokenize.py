# -*- coding:utf-8 -*-
# @author xupingmao
# @since 2016
# @modified 2022/04/10 16:11:15

# MP_TEST
try:
    _ = load
except:
    # need some tool from python to bootstrap
    # TODO: 支持`from boot import load`
    from boot import *

class Token:
    pos = [-1, -1]
    
    def __init__(self, type='symbol', val=None, pos=None):
        self.pos  = pos
        self.type = type
        self.val  = val


def findpos(token: Token):
    if not hasattr(token, 'pos'):
        if hasattr(token, "first"):
            return findpos(token.first)
        print(token)
        return [0,0]
    return token.pos


def find_error_line(s: str, pos: "list[int]"):
    """
    @param {str} s: source code
    @param {int} pos: position
    """
    #print("****************")
    #print(pos, pos.type, pos.val, pos.pos)
    y = pos[0]
    x = pos[1]
    s = s.replace('\t', ' ')
    line = s.split('\n')[y-1]
    p = ''
    if y < 10: p += ' '
    if y < 100: p += '  '
    r = p + str(y) + ": " + line + "\n"
    r += "     "+" "*x+"^" +'\n'
    return r

def print_token(token: Token):
    for key in token:
        print(key, token[key])
        if gettype(token[key]) == "dict":
            print_token(token[key])
    
def compile_error(module_name, s, token, e_msg = ""):
    if token != None:
        # print_token(token)
        pos = findpos(token)
        r = find_error_line(s, pos)
        raise Exception('Error at ' + module_name + ':\n' + r + e_msg)
    else:
        raise Exception(e_msg)
    #raise

SYMBOL_CHARS = '-=[];,./!%*()+{}:<>@^'

KEYWORDS = [
    'as','def','class', 'return','pass','and','or','not','in','import',
    'is','while','break','for','continue','if','else','elif','try',
    'except','raise','global','del','from','None', 'assert'
]

SYMBOLS = [
    '-=','+=','*=','/=','==','!=','<=','>=',
    '->', # typing return
    '=','-','+','*', '/', '%',
    '<','>',
    '[',']','{','}','(',')','.',':',',',';',
]

B_BEGIN = ['[','(','{']
B_END   = [']',')','}']

class Tokenizer:

    def __init__(self):
        self.y=1
        self.yi=0
        self.nl=True
        self.res=[] # type: list[Token]
        self.indent=[0]
        self.braces=0

    def add(self,t,v): 
        if t == 'in':
            last = self.res.pop()
            if last.type == 'not':
                self.res.append(Token('notin', v, self.f))
            else:
                self.res.append(last)
                self.res.append(Token(t,v,self.f))
        elif t == 'not':
            # is not
            last = self.res.pop()
            if last.type == 'is':
                self.res.append(Token("isnot", v, self.f))
            else:
                self.res.append(last)
                self.res.append(Token(t,v,self.f))
        else:
            self.res.append(Token(t,v,self.f))

def clean(s: str):
    s = s.replace('\r','')
    return s

def tokenize(s: str):
    s = clean(s)
    return do_tokenize(s)

def is_blank(c):
    return c == ' ' or c == '\t'

def is_number_begin(c):
    return c >= '0' and c <= '9'

def do_tokenize(s):
    tokenizer = Tokenizer()
    i = 0
    l = len(s)
    while i < l:
        c = s[i]
        tokenizer.f = [tokenizer.y,i-tokenizer.yi+1]
        if tokenizer.nl: 
            tokenizer.nl = False
            i = do_indent(tokenizer,s,i,l)
        elif c == '\n':
            i = do_nl(tokenizer,s,i,l)
        elif c in SYMBOL_CHARS: 
            i = do_symbol(tokenizer,s,i,l)
        elif is_number_begin(c):
            i = do_number(tokenizer,s,i,l)
        elif is_name_begin(c):
            i = do_name(tokenizer,s,i,l)
        elif c == '"' or c == "'":
            i = do_string(tokenizer,s,i,l)
        elif c == '#':
            i = do_comment(tokenizer,s,i,l)
        elif c == '\\' and s[i+1] == '\n':
            i += 2; 
            tokenizer.y+=1; 
            tokenizer.yi = i
        elif is_blank(c): 
            i += 1
        else:
            compile_error('do_tokenize',s, Token('', '', tokenizer.f), "unknown token")
    indent(tokenizer, 0)
    return tokenizer.res

def do_nl(tokenizer, s,i,l):
    if not tokenizer.braces:
        tokenizer.add('nl','nl')
    i+=1
    tokenizer.nl=True
    tokenizer.y+=1
    tokenizer.yi=i
    return i

def do_indent(tokenizer,s,i,l):
    v = 0
    while i<l:
        c = s[i]
        if c != ' ' and c != '\t': 
            break
        i+=1
        v+=1
    # skip blank line or comment line.
    # i >= l means reaching EOF, which do not need to indent or dedent
    if not tokenizer.braces and c != '\n' and c != '#' and i < l:
        indent(tokenizer, v)
    return i

def indent(tokenizer, v):
    if v == tokenizer.indent[-1]:
        return
    elif v > tokenizer.indent[-1]:
        tokenizer.indent.append(v)
        tokenizer.add('indent',v)
    elif v < tokenizer.indent[-1]:
        n = tokenizer.indent.index(v)
        while len(tokenizer.indent) > n+1:
            v = tokenizer.indent.pop()
            tokenizer.add('dedent',v)


def symbol_match(s, i, v):
    return s[i:i+len(v)] == v
            
def do_symbol(tokenizer, s,i,l):
    v = None
    for sb in SYMBOLS:
        if symbol_match(s, i, sb):
            i += len(sb)
            v = sb
            break
    if v == None:
        raise "invalid symbol"
    tokenizer.add(v,v)
    if v in B_BEGIN: 
        tokenizer.braces += 1
    if v in B_END: 
        tokenizer.braces -= 1
    return i

def do_number(tokenizer: Tokenizer, s,i,l):
    v=s[i];i+=1;c=None
    is_float = False
    while i<l:
        c = s[i]
        if (c < '0' or c > '9') and (c < 'a' or c > 'f') and c != 'x': break
        v+=c;i+=1
    if c == '.':
        is_float = True
        v+=c;i+=1
        while i<l:
            c = s[i]
            if c < '0' or c > '9': break
            v+=c;i+=1
    if is_float:
        tokenizer.add('number',float(v))
    else:
        tokenizer.add('number',int(v))
    return i

def is_name_begin(c):
    return (c>='a' and c<='z') or (c>='A' and c<='Z') or (c in '_$')
    
def is_name(c):
    return (c>='a' and c<='z') or (c>='A' and c<='Z') or (c in '_$') or (c>='0' and c<='9')
    
def do_name(tokenizer, s,i,l):
    v=s[i];i+=1
    while i<l:
        c = s[i]
        if not is_name(c): break
        v+=c
        i+=1
    if v in KEYWORDS: 
        tokenizer.add(v,v)
    else: 
        tokenizer.add('name',v)
    return i

def do_string(tokenizer, s,i,l):
    v = ''
    q = s[i]  # quote char
    i += 1
    rest = l - i

    if rest >= 5 and s[i] == q and s[i+1] == q:
        # check long string """
        i += 2
        while i<l-2:
            c = s[i]
            if c == q and s[i+1] == q and s[i+2] == q:
                i += 3
                tokenizer.add('string',v)
                break
            else:
                v+=c; i+=1
                if c == '\n': 
                    tokenizer.y += 1
                    tokenizer.x = i
    else:
        while i<l:
            c = s[i]
            if c == "\\":
                i = i+1; c = s[i]
                if c == "n": c = '\n'
                elif c == "r": c = chr(13)
                elif c == "t": c = "\t"
                elif c == "0": c = "\0"
                elif c == 'b': c = '\b'
                v+=c;i+=1
            elif c == q:
                i += 1
                tokenizer.add('string',v)
                break
            else:
                v+=c;i+=1
    return i

def do_comment(tokenizer, s,i,l):
    i += 1
    value = ""
    while i<l:
        if s[i] == '\n': break
        value += s[i]
        i += 1
    if value.startswith("@debugger"):
        tokenizer.add("@", "debugger")
    return i

# MP_TEST
def _main():
    import sys
    ARGV = sys.argv
    if len(ARGV) != 2:
        print("error arguments, arguments = ", ARGV)
        return
    fname = ARGV[1]
    print("tokenize file: %s ..." % fname)
    content = load(fname)
    list = tokenize(content)
    for i in list:
        info = "%s %s %r" % (i.pos, i.type, i.val)
        print(info)

if __name__ == "__main__":
    _main()
# MP_TEST_END


