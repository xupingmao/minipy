# -*- coding:utf-8 -*-
# @author xupingmao
# @since 2022/04/10 14:33:07
# @modified 2022/04/11 19:03:07
# @filename test_tokenize.py
import os
import sys

sys.path.append("src/python")
sys.path.append("test/test_tokenize")

import mp_tokenize


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
        if os.path.isdir(fpath):
            continue
        filename = filename.split(".py")[0]
        mod = __import__(filename)
        tk_test(filename, mod.input_text, mod.output)

if __name__ == '__main__':
    main()
