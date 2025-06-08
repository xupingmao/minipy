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
    
def compile_error(module_name, code:str, token, e_msg = ""):
    if token != None:
        # print_token(token)
        pos = findpos(token)
        r = find_error_line(code, pos)
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

# 从前往后优先匹配
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

    f = [-1,-1] # position

    def __init__(self, code=""):
        self.y=1
        self.yi=0
        self.nl=True
        self.res=[] # type: list[Token]
        self.indent_stack = [0]
        self.braces=0
        self.code = code

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

    def tokenize(self):
        i = 0
        s = self.code
        l = len(self.code)
        while i < l:
            c = s[i]
            self.f = [self.y,i-self.yi+1]
            if self.nl: 
                self.nl = False
                i = self.do_indent(s,i,l)
            elif c == '\n':
                i = self.do_nl(s,i,l)
            elif c in SYMBOL_CHARS: 
                i = self.do_symbol(s,i,l)
            elif is_number_begin(c):
                i = self.do_number(s,i,l)
            elif is_name_begin(c):
                i = self.do_name(s,i,l)
            elif c == '"' or c == "'":
                i = self.do_string(s,i,l)
            elif c == '#':
                i = self.do_comment(s,i,l)
            elif c == '\\' and s[i+1] == '\n':
                i += 2; 
                self.y+=1; 
                self.yi = i
            elif is_blank(c): 
                i += 1
            else:
                compile_error('do_tokenize',s, Token('', '', self.f), "unknown token")
        self.indent(0)


    def do_indent(self,s,i,l):
        v = 0
        while i<l:
            c = s[i]
            if c != ' ' and c != '\t': 
                break
            i+=1
            v+=1
        # skip blank line or comment line.
        # i >= l means reaching EOF, which do not need to indent or dedent
        if not self.braces and c != '\n' and c != '#' and i < l:
            self.indent(v)
        return i

    def indent(self, v):
        if v == self.indent_stack[-1]:
            return
        elif v > self.indent_stack[-1]:
            self.indent_stack.append(v)
            self.add('indent',v)
        elif v < self.indent_stack[-1]:
            n = self.indent_stack.index(v)
            while len(self.indent_stack) > n+1:
                v = self.indent_stack.pop()
                self.add('dedent',v)


    def do_nl(self, s,i,l):
        if not self.braces:
            self.add('nl','nl')
        i+=1
        self.nl=True
        self.y+=1
        self.yi=i
        return i

    
    def do_symbol(self, s:str, i:int, l:int):
        v = ""
        for sb in SYMBOLS:
            if s.startswith(sb, i):
                i += len(sb)
                v = sb
                break
        if v == "":
            raise "invalid symbol"
        
        self.add(v,v)
        if v in B_BEGIN: 
            self.braces += 1
        if v in B_END: 
            self.braces -= 1
        return i

    def do_number(self, s,i,l):
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
            self.add('number',float(v))
        else:
            self.add('number',int(v))
        return i

    
    def do_name(self, s,i,l):
        v=s[i];i+=1
        while i<l:
            c = s[i]
            if not is_name(c): break
            v+=c
            i+=1
        if v in KEYWORDS: 
            self.add(v,v)
        else: 
            self.add('name',v)
        return i

    def do_string(self, s,i,l):
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
                    self.add('string',v)
                    break
                else:
                    v+=c; i+=1
                    if c == '\n': 
                        self.y += 1
                        self.x = i
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
                    self.add('string',v)
                    break
                else:
                    v+=c;i+=1
        return i

    def do_comment(self, s,i,l):
        i += 1
        value = ""
        while i<l:
            if s[i] == '\n': break
            value += s[i]
            i += 1
        if value.startswith("@debugger"):
            self.add("@", "debugger")
        return i

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
    tokenizer = Tokenizer(s)
    tokenizer.tokenize()
    return tokenizer.res
          

def is_name_begin(c):
    return (c>='a' and c<='z') or (c>='A' and c<='Z') or (c in '_$')
    
def is_name(c):
    return (c>='a' and c<='z') or (c>='A' and c<='Z') or (c in '_$') or (c>='0' and c<='9')
   
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


