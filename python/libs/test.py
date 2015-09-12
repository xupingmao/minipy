from boot import *

def test0():
    print("test")

def print_func_call(func, args):
    if str(1.0) == '1' and func.__self__:
        printf(quote(func.__self__))
        printf(".")
    printf(func.func_name)
    printf('(')
    if args == None:
        printf(')')
        return
    i = 0
    while i < len(args):
        printf(quote(args[i]))
        i+=1
        if i != len(args):
            printf(',')
    printf(')')


def protected_run(func, args):
    try:
        apply(func, args)
    except Exception as e:
        printf("FAIL -- %s\n", e)

        
def testfunc(func, args, tar=None, show_time = 0):
    try:
        t1 = clock()
        r = apply(func, args)
        t2 = clock()
        if tar != None and r != tar:
            printf("RUN ")
            print_func_call(func, args)
            printf(" FAIL\n")
            printf("  expect %s but result is %s\n", quote(tar), 
                quote(r))
        else:
            printf("RUN ")
            print_func_call(func, args)
            printf('=%s\n', quote(r))
        if show_time:
            printf("cost time = %s\n", (t2-t1))
    except Exception as e:
        printf("RUN ")
        print_func_call(func, args)
        printf(' FAIL\n  Exception:%s\n', quote(e))
        # printf("hello,world\n");