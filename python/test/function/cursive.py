from test import *
def cursive(n):
    if n > 0:
        cursive(n-1)

test(cursive, [200], None, 1)
cursive(200)