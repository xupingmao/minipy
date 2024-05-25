# -*- coding:utf-8 -*-
'''
Author: xupingmao
email: 578749341@qq.com
Date: 2023-12-07 22:03:29
LastEditors: xupingmao
LastEditTime: 2024-05-25 11:28:40
FilePath: /minipy/test/test_case/051_test_print_vm_info.py
Description: 描述
'''
import mp_debug
import repl

repl.repl_print(mp_debug.get_vm_info())
