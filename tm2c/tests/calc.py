
def prog():
    next()
    item()
    while (lookahead == '+' or lookahead == '-'):
        next()
        item()

def item():
    factor()
    while (lookahead == '*' or lookahead == '/'):
        next()
        factor()

def factor():
    next("number")


if __name__ == "__main__":
    prog()