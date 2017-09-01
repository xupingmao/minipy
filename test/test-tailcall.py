
def tailcall(n):
    # print(n)
    if n > 0:
        return tailcall(n-1)

tailcall(2000)

