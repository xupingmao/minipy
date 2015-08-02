def printf0(args):
    fmt = args[0]
    fmt_len = len(fmt)
    i = 0
    args_cnt = 0
    is_char = 1
    while i < fmt_len:
        is_char = 1
        if fmt[i] == '%':
            i+=1
            c = fmt[i]
            if c == 's' or c == 'd' or c == 'o':
                args_cnt += 1
                write(args[args_cnt])
            elif c in '1234567890':
                args_cnt += 1
                n = 0
                while fmt[i] in '1234567890':
                    n = n * 10 + ord(fmt[i]) - ord('0')
                    i += 1
                if fmt[i] == 's':
                    write(str(args[args_cnt]).rjust(n))
                else:
                    raise("format error")
            else:
                write(fmt[i])
                i-=1
        else:
            write(fmt[i])
        i += 1

def printf(*args):
    n = len(args)
    if n == 1:
        write(sformat(args[0]))
    elif n == 2:
        write(sformat(args[0], args[1]))
    elif n == 3:
        write(sformat(args[0], args[1], args[2]))
    elif n == 4:
        write(sformat(args[0], args[1], args[2], args[3]))
    #arg_num = len(args)
    #if arg_num == 1:
        #write(args[0])
    #else:
        #printf0(args)
add_builtin("printf", printf)

def uncode16(a,b):
    return ord(a) * 256 + ord(b)
add_builtin("uncode16", uncode16)


class range:
    def __init__(self, arg0 = None, arg1 = None, arg2 = None):
        if not arg2:
            if not arg1:
                start = 0
                stop = arg0
                inc = 1
            else:
                start = arg0
                stop = arg1
                inc = 1
        else:
            start = arg0
            stop = arg1
            inc = arg2
        if arg0 == None:
            raise("range: require at least 1 parameter!")
        self.cur = start
        self.stop = stop
        self.inc = inc
    
    def __iter__(self):
        return self
    def next(self):
        cur = self.cur
        inc = self.inc
        if inc > 0:
            if cur < self.stop:
                self.cur += inc
                return cur
        elif inc < 0:
            if cur > self.stop:
                self.cur += inc
                return cur
        raise

__builtins__.range = range

def dir(obj):
    if istype(obj, "string"):return __strclass__.keys()
    elif istype(obj, "function"):return __funcclass__.keys()
    elif istype(obj, "number"):return None

def mtime(fname):
    obj = stat(fname)
    return obj.st_mtime
add_builtin("mtime", mtime)

loadlib("libs/object/string.py")
loadlib("libs/object/dict.py")
