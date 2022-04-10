# -*- coding:utf-8 -*-
# @author xupingmao
# @since 2022/04/10 14:33:07
# @modified 2022/04/10 15:10:27
# @filename test_tokenize.py
import os
import sys
import six


class Storage(dict):
    """
    A Storage object is like a dictionary except `obj.foo` can be used
    in addition to `obj['foo']`. (This class is modified from web.py)
    
        >>> o = storage(a=1)
        >>> o.a
        1
        >>> o['a']
        1
        >>> o.a = 2
        >>> o['a']
        2
        >>> o.noSuchKey
        None
    """
    def __init__(self, **kw):
        # default_value会导致items等函数出问题
        # self.default_value = default_value
        super(Storage, self).__init__(**kw)

    def __getattr__(self, key):
        try:
            return self[key]
        except KeyError as k:
            return AttributeError(k)
    
    def __setattr__(self, key, value): 
        self[key] = value
    
    def __delattr__(self, key):
        try:
            del self[key]
        except KeyError as k:
            raise AttributeError(k)

    def __deepcopy__(self, memo):
        return Storage(**self)
    
    def __repr__(self):     
        return '<MyStorage ' + dict.__repr__(self) + '>'

def load_module(fpath, **kw):
    with open(fpath) as fp:
        code = fp.read()
        name = "minipy_" + fpath
        name = name.replace("/", "_")
        name = name.replace(".", "_")

        vars = {
            "__name__": name,
            "__file__": fpath,
        }
        vars.update(**kw)
        six.exec_(code, vars)
        return Storage(**vars)


def load(fpath):
    with open(fpath) as fp:
        return fp.read()

mp_tokenize = load_module("src/python/mp_tokenize.py", load=load)

def tk_types(r):
    rr = []
    for tk in r:
        rr.append(tk.type)
    return rr
    
def tk_vals(r):
    rr = []
    for tk in r:
        rr.append(tk.val)
    return rr
    
def tk_test(filename, string, tokens):
    print("test:", filename)
    r = mp_tokenize.tokenize(string)
    for i in range(len(r)):
        tk = r[i]
        assert tk.type == tokens[i*2],   (tk.type, tokens[i*2])
        assert tk.val  == tokens[i*2+1], (tk.val, tokens[i*2+1])

def main():
    dirname = "test/test_tokenize"
    for filename in os.listdir(dirname):
        fpath = os.path.join(dirname, filename)
        mod = load_module(fpath)
        tk_test(filename, mod.input_text, mod.output)

if __name__ == '__main__':
    main()
