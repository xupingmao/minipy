
'''
abc -> state = 1
[1,2,3] -> state = 2, collection = [1,2,3]
a* -> state = 3, pre = 'a'
a+ -> state = 4, pre = 'a'
a? -> state = 4, pre = 'a'
'''

_ANY = 0
_ONE = 2
_STAR = 3
_SET = 4
_PLUS = 5
_CHAR = 6
_GROUP = 7

class RegError:
    pass
    
class RegItem:
    def __init__(self, pattern, i, pre):
        c = pattern[i]
        self.pre = pre
        self.value = pattern[i]
        if c == '.':
            self.state = _ANY
            self.i = i + 1
        elif c == '?':
            assert pre
            self.state = _ONE
            self.i = i + 1
        elif c == '*':
            assert pre
            self.state = _STAR
            self.i = i + 1
        elif c == '+':
            assert pre
            self.state = _PLUS
            self.i = i + 1
        elif c == '[' or c == '(':
            if c == '[':
                end = ']'
                self.state = _SET
            else:
                end = ')'
                self.state = _GROUP
            i += 1
            pre = None
            set = []
            while i < len(pattern) and pattern[i] != end:
                pre = RegItem(pattern, i, pre)
                set.append(pre)
                i = pre.i
            self.set = set
            if pattern[i] == end:
                i += 1
            self.i = i
        elif c == '\\':
            self.state = _CHAR
            self.value = pattern[i+1]
            self.i = i + 2
        else:
            self.state = _CHAR
            self.value = c
            self.i = i + 1

def compile(pattern):
    i = 0
    pre = None
    set = []
    while i < len(pattern):
        pre = RegItem(pattern, i, pre)
        set.append(pre)
        i = pre.i
    return set
        
def match(text, pattern):
    re_compile(pattern).match(text)
    
def findall(text, pattern):
    return compile(pattern).parse(text)
        
if __name__ == "__main__":
    code = compile("abc?+test[123].*")
    for i in code:
        print(i.state, i.value, i.i, i.pre)
        
        