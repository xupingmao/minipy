# test tools
import logging
import sys

is_minipy = True

try:
    import mp_debug
except ImportError:
    sys.path.append("src/python")
    from boot import istype
    from boot import apply
    is_minipy = False

class Result:

    def __init__(self, success, result, exception = None):
        self.success = success
        self.result = result
        self.exception = exception

def toPrintableStr(obj):
    if istype(obj, 'string'):
        obj = obj.replace('\t', '\\t')
        obj = obj.replace('\n', '\\n')
        obj = obj.replace('\r', '')
        return '[' + obj + ']'
    elif istype(obj, 'number'):
        return str(obj)
    return str(obj)
    
def getFuncName(func):
    return func.__name__
        
        
def testException(func, args, expect = None):
    try:
        logging.info('RUN', getFuncName(func), args)
        ret = apply(func, args)
        logging.error("FAIL")
    except Exception as e:
        assertEquals(e, expect)
        
def testExceptionType(func, args, expect = None):
    try:
        logging.info('RUN', getFuncName(func), args)
        ret = apply(func, args)
        logging.error("FAIL")
    except Exception as e:
        assertStarts(e, expect)
        
def testfunc(func, args, expect = None):
    try:
        logging.info('RUN', getFuncName(func), args)
        ret = apply(func, args)
        if expect != None and not assertEquals(ret, expect, 'testfunc'):
            return Result(False, ret)
        return Result(True, ret)
    except Exception as e:
        return Result(False, e, e)
        
def assertEquals(result, expect, msg=None):
    if result == expect:
        logging.info('PASS, result:' + toPrintableStr(result))
        return True
    else:
        logging.error('FAIL, result:' + toPrintableStr(result),\
            'expect:' + toPrintableStr(expect), "msg:",msg)
        logging.error("result.type:" + gettype(result), "expect.type:" + gettype(result))
        raise
        
def assertStarts(result, expect, msg=None):
    if result.startswith(expect):
        logging.info('PASS, result=' + toPrintableStr(result))
        return True
    else:
        logging.error('FAIL, result=' + toPrintableStr(result),\
            'expect=', toPrintableStr(expect), msg)
        raise
        
def assertTrue(value, msg=None):
    if not value:
        error_msg = 'FAIL, msg=(%s)' % msg
        logging.error(error_msg)
        raise Exception(error_msg)


