
# TM_TEST
if "tm" not in globals():
    # need some tool from python to bootstrap
    from boot import *
# TM_TEST_END

    
# keep Token as a pure function
def Token(type='symbol',val=None,pos=None):
    self = newobj()
    self.pos=pos
    self.type=type
    self.val=val
    return self

def findpos(token):
    if not hasattr(token, 'pos'):
        if hasattr(token, "first"):
            return findpos(token.first)
        print(token)
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
        print(key, token[key])
        if gettype(token[key]) == "dict":
            print_token(token[key])
    
def compile_error(ctx, s, token, e_msg = ""):
    if token != None:
        # print_token(token)
        pos = findpos(token)
        r = find_error_line(s, pos)
        raise Exception('Error at '+ctx+':\n'+r + e_msg)
    else:
        raise Exception(e_msg)
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

class TData:
    def __init__(self):
        self.y=1
        self.yi=0
        self.nl=True
        self.res=[]
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
        else:
            self.res.append(Token(t,v,self.f))

def clean(s):
    s = s.replace('\r','')
    return s

def tokenize(s):
    global T
    s = clean(s)
    return do_tokenize(s)
        
def do_tokenize(s):
    global T
    T = TData()
    i = 0
    l = len(s)
    while i < l:
        c = s[i]
        T.f = [T.y,i-T.yi+1]
        if T.nl: 
            T.nl = False
            i = do_indent(s,i,l)
        elif c == '\n': i = do_nl(s,i,l)
        elif c in ISYMBOLS: i = do_symbol(s,i,l)
        elif c >= '0' and c <= '9': i = do_number(s,i,l)
        elif (c >= 'a' and c <= 'z') or \
            (c >= 'A' and c <= 'Z') or c == '_':  i = do_name(s,i,l)
        elif c=='"' or c=="'": i = do_string(s,i,l)
        elif c=='#': i = do_comment(s,i,l)
        elif c == '\\' and s[i+1] == '\n':
            i += 2; T.y+=1; T.yi = i
        elif c == ' ' or c == '\t': i += 1
        else: compile_error('do_tokenize',s, Token('', '', T.f), "unknown token")
    indent(0)
    r = T.res
    T = None
    return r

def do_nl(s,i,l):
    if not T.braces:
        T.add('nl','nl')
    i+=1
    T.nl=True
    T.y+=1
    T.yi=i
    return i

def do_indent(s,i,l):
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
        indent(v)
    return i

def indent(v):
    if v == T.indent[-1]: pass
    elif v > T.indent[-1]:
        T.indent.append(v)
        T.add('indent',v)
    elif v < T.indent[-1]:
        n = T.indent.index(v)
        while len(T.indent) > n+1:
            v = T.indent.pop()
            T.add('dedent',v)


def do_symbol(s,i,l):
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
    T.add(v,v)
    if v in B_BEGIN: T.braces += 1
    if v in B_END: T.braces -= 1
    return i

def do_number(s,i,l):
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
    T.add('number',float(v))
    return i

def do_name(s,i,l):
    v=s[i];i+=1
    while i<l:
        c = s[i]
        if (c < 'a' or c > 'z') and (c < 'A' or c > 'Z') and (c < '0' or c > '9') and c != '_': break
        v+=c; i+=1
    if v in KEYWORDS: T.add(v,v)
    else: T.add('name',v)
    return i

def do_string(s,i,l):
    v = ''; q=s[i]; i+=1
    if (l-i) >= 5 and s[i] == q and s[i+1] == q: # """
        i += 2
        while i<l-2:
            c = s[i]
            if c == q and s[i+1] == q and s[i+2] == q:
                i += 3
                T.add('string',v)
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
                T.add('string',v)
                break
            else:
                v+=c;i+=1
    return i

def do_comment(s,i,l):
    i += 1
    value = ""
    while i<l:
        if s[i] == '\n': break
        value += s[i]
        i += 1
    if value.startswith("@debugger"):
        T.add("@", "debugger")
    return i

# TM_TEST
def _main():
    if len(ARGV) != 2:
        print("error arguments, arguments = ", ARGV)
        return
    fname = ARGV[1]
    print("try to tokenize file ", fname)
    content = load(fname)
    list = tokenize(content)
    for i in list:
        print(i.pos, i.type, i.val)

if __name__ == "__main__":
    _main()
# TM_TEST_END


