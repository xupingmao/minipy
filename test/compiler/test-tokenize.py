# -*- coding:utf-8 -*-
# @author xupingmao <578749341@qq.com>
# @since 2020/10/20 00:14:13
# @modified 2020/10/20 00:19:09
import sys
sys.path.append("src/python")

from mp_tokenize import *


# assign
tokens = tokenize("a = 10")
assert len(tokens) == 3
assert tokens[0].type == "name"
assert tokens[1].type == "="
assert tokens[2].type == "number"

