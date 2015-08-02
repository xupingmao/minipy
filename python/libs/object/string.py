def ljust(self, num):
    num = int(num)
    if len(self) >= num: return self
    rest = num - len(self)
    return self + rest * ' '
def rjust(self, num):
    num = int(num)
    if len(self) >= num: return self
    rest = num - len(self)
    return rest * ' ' + self
def center(self, num):
    num = int(num)
    if len(self) >= num: return self
    right = (num - len(self)) / 2
    right = int(right)
    left = num - right - len(self)
    return left * ' ' + self + right * ' '
def startswith(self,start):
    return self.find(start)==0
def endswith(self, end):
    idx = self.find(end)
    if idx < 0: return False
    return idx + len(end) == len(self)
    
def slice(src, start, end):
    pass

add_obj_method('str', 'ljust', ljust)
add_obj_method('str', 'rjust', rjust)
add_obj_method('str', 'center', center)
add_obj_method('str', 'startswith', startswith)
add_obj_method('str', 'endswith', endswith)

def escape(text):
    if gettype(text) != 'string':
        raise "<function escape> expect a string"
    des = ""
    for c in text:
        if c == '\r':des+='\\r'
        elif c == '\n':des+='\\n'
        elif c == '\b':des += '\\b'
        elif c == '\\':des += '\\\\'
        elif c == '\"':des += '\\"'
        elif c == '\0':des += '\\0'
        else:des+=c
    return des

add_builtin("escape", escape)

def quote(obj):
    if istype(obj, 'string'):
        return '"' + escape(obj) + '"'
    else:
        return str(obj)
add_builtin("quote", quote)

def sformat(*args):
    fmt = args[0]
    fmt_len = len(fmt)
    if fmt_len == 1: return fmt
    i = 0
    args_cnt = 0
    is_char = 1
    dest = ""
    left = 0
    while i < fmt_len:
        is_char = 1
        if fmt[i] == '%':
            i+=1
            c = fmt[i]
            if c == '-' and (i+1 < fmt_len) and fmt[i+1] in '1234567890':
                i += 1
                c = fmt[i]
                left = 1
            if c in ('s', 'd', 'o'):
                flag = c
                args_cnt += 1
                dest+=str(args[args_cnt])
            elif c in '1234567890':
                n = 0
                while fmt[i] in '1234567890':
                    n = n * 10 + ord(fmt[i]) - ord('0')
                    i += 1
                if fmt[i] == 's':
                    args_cnt += 1
                    if left:
                        dest+=str(args[args_cnt]).ljust(n)
                        left = 0
                    else:
                        dest+=str(args[args_cnt]).rjust(n)
                else:
                    raise("format error")
            else:
                dest+=fmt[i]
                i-=1
        else:
            dest+=fmt[i]
        i += 1
    return dest
add_builtin("sformat", sformat)
