# -*- coding:utf-8 -*-
'''
Author: xupingmao
email: 578749341@qq.com
Date: 2021-09-29 23:28:26
LastEditors: xupingmao
LastEditTime: 2024-06-02 01:39:25
FilePath: /minipy/test/logging.py
Description: 描述
'''

import sys
DEBUG = False

try:
    import mp_debug
except ImportError:
    sys.path.append("src/python")
    from boot import istype

def toBaseStr(obj):
    if istype(obj, 'string'):
        result = obj
    elif istype(obj, 'number'):
        result = str(obj)
    else:
        result = str(obj)
    
    if DEBUG:
        print("toBaseStr", obj, "->", result)
    return result

def toDictStr(dict):
    dictStr = '{'
    for key in dict:
        if dictStr != '{':
            dictStr += ','
        dictStr += toBaseStr(key) + ':' + toBaseStr(dict[key])
    dictStr += '}'
    if DEBUG:
        print("toDictStr", dict, "->", dictStr)
    return dictStr

def toLoggingStr(obj):
    if istype(obj, 'dict'):
        result = toDictStr(obj)
    else:
        result = toBaseStr(obj)
    if DEBUG:
        print("toLoggingStr", obj, "->", result)
    return result

# @list args
def _log(level, args):
    # print("_log", level, args)
    
    newArgs = []
    for arg in args:
        new_arg = toLoggingStr(arg)
        newArgs.append(new_arg)
    
    # print("_log", level, newArgs)

    sArgs = " ".join(newArgs)
    print(level, ":", sArgs)

def info(*args):
    _log("INFO", args)
    
def debug(*args):
    _log("DEBUG", args)
    
def error(*args):
    _log("ERROR", args)
    
def logCall(func, args):
    _log('CALL', [func.__name__, args])
    