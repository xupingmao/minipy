# -*- coding:utf-8 -*-
# @author xupingmao
# @since 2022/04/10 15:06:25
# @modified 2022/04/10 15:10:04
# @filename case2.py

input_text = """
def foo(bar):
    return bar * 5
"""

output = [
    "nl", "nl",
    "def", "def",
    "name", "foo",
    "(", "(",
    "name", "bar",
    ")", ")",
    ":", ":",
    "nl", "nl",
    "indent", 4,
    "return", "return",
    "name", "bar",
    "*", "*",
    "number", 5,
    "nl", "nl",
    "dedent", 4,
]
