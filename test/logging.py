
def toBaseStr(obj):
    if istype(obj, 'string'):
        return obj
    elif istype(obj, 'number'):
        return str(obj)
    return str(obj)

def toDictStr(dict):
    dictStr = '{'
    for key in dict:
        if dictStr != '{':
            dictStr += ','
        dictStr += toBaseStr(key) + ':' + toBaseStr(dict[key])
    dictStr += '}'
    return dictStr

def toLoggingStr(obj):
    if istype(obj, 'dict'):
        return toDictStr(obj)
    return toBaseStr(obj)
# @list args
def _log(level, args):
    newArgs = []
    for arg in args:
        newArgs.append(toLoggingStr(arg))
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
    