
def pcall(func, args, p = 0):
    try:
        r = apply(func, args)
        if p:
            printf("SUCCESS, result is (%s)\n", r)
        return r
    except Exception as e:
        if p:
            printf("FAIL, exception is (%s)\n", e)
        return 0