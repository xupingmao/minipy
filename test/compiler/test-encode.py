# -*- coding:utf-8 -*-
# @author xupingmao <578749341@qq.com>
# @since 2020/10/20 00:19:47
# @modified 2020/10/20 00:42:52

import sys
sys.path.append("src/python")

from mp_encode import *


def test_compile(fname):
    input_fname  = "test/compiler/case/%s-input.py" % fname
    output_fname = "test/compiler/case/%s-output.txt" % fname

    code   = load(input_fname)
    expect = load(output_fname)

    bytecode = dis_code(code, True)
    if expect != bytecode:
        print("expect:\n%s \n\nactual:\n%s\n" % (expect, bytecode))
        raise Exception("assert failed, fname=%s" % fname)

test_compile("assign-number")
test_compile("assign-multi")
test_compile("if-in")
