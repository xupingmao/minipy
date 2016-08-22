from boot import *

def Token(type='symbol',val=None,pos=None):
    o = newobj()
    o.pos=pos
    o.type=type
    o.val=val
    return o

def findpos(token):
    if not hasattr(token, 'pos'):
        if hasattr(token, "first"):
            return findpos(token.first)
        # print(token)
        return [0,0]
    return token.pos
# @param s, src
# @param pos, position
def find_error_line(s, pos):
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

def print_token(token):
    for key in token:
        # print(key, token[key])
        if gettype(token[key]) == "dict":
            print_token(token[key])
    
def compile_error(ctx, s, token, e_msg = ""):
    if token != None:
        # print_token(token)
        pos = findpos(token)
        r = find_error_line(s, pos)
        print('Error at '+ctx+':\n'+r + str(e_msg))
    else:
        print(e_msg)
    exit(0)
    #raise

ISYMBOLS = '-=[];,./!%*()+{}:<>@^$'
KEYWORDS = [
    'as','def','class', 'return','pass','and','or','not','in','import',
    'is','while','break','for','continue','if','else','elif','try',
    'except','raise','global','del','from','None', "assert"]
SYMBOLS = [
    '-=','+=','*=','/=','==','!=','<=','>=',
    '=','-','+','*', '/', '%', #'**','/','%','<<','>>',
    '<','>',
    '[',']','{','}','(',')','.',':',',',';',
    # "@",
    #,'&', '|','!','@','^','$'
    ]
B_BEGIN = ['[','(','{']
B_END = [']',')','}']

def TData():
    self = newobj()
    self.y=1
    self.yi=0
    self.nl=True
    self.res=[]
    self.indent=[0]
    self.braces=0
    return self

def add_token(self,t,v): 
    if t == 'in':
        last = self.res.pop()
        if last.type == 'not':
            self.res.append(Token('notin', v, self.f))
        else:
            self.res.append(last)
            self.res.append(Token(t,v,self.f))
    else:
        self.res.append(Token(t,v,self.f))

def clean(s):
    s = s.replace('\r','')
    return s

def tokenize(s):
    s = clean(s)
    return do_tokenize(s)
        
def do_tokenize(s):
    T = TData()
    i = 0
    l = len(s)
    while i < l:
        c = s[i]
        T.f = [T.y,i-T.yi+1]
        if T.nl: 
            T.nl = False
            i = do_indent(T, s,i,l)
        elif c == '\n': i = do_nl(T, s,i,l)
        elif c in ISYMBOLS: i = do_symbol(T, s,i,l)
        elif c >= '0' and c <= '9': 
            i = do_number(T,s,i)
        elif (c >= 'a' and c <= 'z') or \
            (c >= 'A' and c <= 'Z') or c == '_': 
            i = do_name(T,s,i)
        elif c=='"' or c=="'": i = do_string(T,s,i,l)
        elif c=='#': i = do_comment(T,s,i,l)
        elif c == '\\' and s[i+1] == '\n':
            i += 2; T.y+=1; T.yi = i
        elif c == ' ' or c == '\t': i += 1
        else: compile_error('do_tokenize',s, Token('', '', T.f), "unknown token")
    indent(T, 0)
    r = T.res
    T = None
    return r

def do_nl(T, s,i,l):
    if not T.braces:
        add_token(T,'nl','nl')
    i+=1
    T.nl=True
    T.y+=1
    T.yi=i
    return i

def do_indent(T, s,i,l):
    v = 0
    while i<l:
        c = s[i]
        if c != ' ' and c != '\t': 
            break
        i+=1
        v+=1
    # skip blank line or comment line.
    # i >= l means reaching EOF, which do not need to indent or dedent
    if not T.braces and c != '\n' and c != '#' and i < l:
        indent(T, v)
    return i

def indent(T, v):
    if v == T.indent[-1]: pass
    elif v > T.indent[-1]:
        T.indent.append(v)
        add_token(T,'indent',v)
    elif v < T.indent[-1]:
        n = T.indent.index(v)
        while len(T.indent) > n+1:
            v = T.indent.pop()
            add_token(T,'dedent',v)


def do_symbol(T, s,i,l):
    # symbols = []
    # v,f,i = s[i],i,i+1
    # v=s[i];f=i;i+=1
    v = None
    for sb in SYMBOLS:
        if mmatch(s, i, sb):
            i += len(sb)
            v = sb
            break
    if v == None:
        raise "invalid symbol"
    #if v in SYMBOLS: symbols.append(v)
    #while i<l:
    #    c = s[i]
    #    if c not in ISYMBOLS: break
    #    # v,i = v+c,i+1
    #    v+=c;i+=1
    #    if v in SYMBOLS: symbols.append(v)
    #v = symbols.pop(); n = len(v); i = f+n
    add_token(T,v,v)
    if v in B_BEGIN: T.braces += 1
    if v in B_END: T.braces -= 1
    return i

def do_number(T, s,i):
    l = len(s)
    v=s[i];i+=1;c=None
    while i<l:
        c = s[i]
        if (c < '0' or c > '9') and (c < 'a' or c > 'f') and c != 'x': break
        v+=c;i+=1
    if c == '.':
        v+=c;i+=1
        while i<l:
            c = s[i]
            if c < '0' or c > '9': break
            v+=c;i+=1
    add_token(T,'number',float(v))
    return i

def do_name(T, s,i):
    l = len(s)
    v=s[i];i+=1
    while i<l:
        c = s[i]
        if (c < 'a' or c > 'z') and (c < 'A' or c > 'Z') and (c < '0' or c > '9') and c != '_': break
        v+=c; i+=1
    if v in KEYWORDS: add_token(T,v,v)
    else: add_token(T,'name',v)
    return i

def do_string(T, s,i,l):
    v = ''; q=s[i]; i+=1
    if (l-i) >= 5 and s[i] == q and s[i+1] == q: # """
        i += 2
        while i<l-2:
            c = s[i]
            if c == q and s[i+1] == q and s[i+2] == q:
                i += 3
                add_token(T,'string',v)
                break
            else:
                v+=c; i+=1
                if c == '\n': T.y += 1;T.x = i
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
                add_token(T,'string',v)
                break
            else:
                v+=c;i+=1
    return i

def do_comment(T, s,i,l):
    i += 1
    value = ""
    while i<l:
        if s[i] == '\n': break
        value += s[i]
        i += 1
    if value.startswith("@debugger"):
        add_token(T,"@", "debugger")
    elif value.startswith("@pass"):
        add_token(T,"@", "pass")
    return i

def _main():
    import sys
    argv = sys.argv
    if len(argv) != 2:
        print("error arguments, arguments = ", argv)
        return
    fname = argv[1]
    content = load(fname)
    list = tokenize(content)
    for i in list:
        print(i.pos, i.type, i.val)

if __name__ == "__main__":
    _main()
