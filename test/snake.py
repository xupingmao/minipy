
def _printf(fmt, *rest):
    print(fmt % rest)

def _sleep(n):
    time.sleep(n / 1000)

def _clock():
    return time.time() * 1000

def _write(string):
    sys.stdout.write(str(string))

if str(1.0) == "1.0":
    import random
    import os
    import sys
    import time
    random = random.random
    system = os.system
    write = _write
    sleep = _sleep
    clock = _clock
    printf = _printf
else:
    import time
    clock = time.clock()

def make_list(cnt):
    list = []
    for x in range(cnt):
        list.append(0)
    return list

class Node:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.value = "o"

class Snake:

    def __init__(self):
        self.dir = "up"
        self.head = Node(0, 0)
        self.nodes = [self.head, Node(1,0), Node(2,0), Node(3,0)]
        self.world = None

    def draw(self):
        for node in self.nodes:
            self.drawNode(node)

    def newNode(self):
        newNode = Node(self.head.x, self.head.y)
        tryCnt = 0;
        while True:
            i = int(random() * 2.9)
            tryCnt += 1
            if tryCnt > 100:
                raise "try maxium exceeded"
            if self.dir == "up":
                self.dir = ["up", "left", "right"][i]
            elif self.dir == "down":
                self.dir = ["down", "left", "right"][i]
            elif self.dir == "left":
                self.dir = ["up", "down", "left"][i]
            else:
                self.dir = ["up", "down", "right"][i]

            # check bundary
            if self.dir == "up":
                if self.head.y + 1 < self.world.getHeight() and \
                    self.world.getxy(newNode.x, newNode.y+1) == 0:
                    newNode.y += 1
                    newNode.value = "^"
                    break
            if self.dir == "left" and \
                self.world.getxy(newNode.x-1, newNode.y) == 0:
                if self.head.x > 0:
                    newNode.x -= 1
                    newNode.value = "<"
                    break
            if self.dir == "right":
                if self.head.x < self.world.getWidth() and \
                    self.world.getxy(newNode.x + 1, newNode.y) == 0:
                    newNode.x += 1
                    newNode.value = ">"
                    break;
            if self.dir == "down":
                if self.head.y > 0 and \
                    self.world.getxy(newNode.x, newNode.y-1) == 0:
                    newNode.y -=1
                    newNode.value = "v"
                    break
        return newNode

    def clearNode(self, node):
        self.world.setxy(node.x, node.y, 0)

    def drawNode(self, node):
        self.world.setxy(node.x, node.y, node.value)

    def move(self):
        self.head.value = "o"
        self.head = self.newNode()
        tail = self.nodes.pop()
        self.clearNode(tail)
        self.nodes.insert(0, self.head)
        for node in self.nodes:
            self.drawNode(node)


class Matrix:
    # m  [x  , y] 
    #    row, col
    # just like array in c lang
    def __init__(self, cols, rows, init = 0):
        self.cols = cols
        self.rows = rows
        self.consolecols = 80
        self.values = make_list(cols * rows)
        for i in range(len(self.values)):
            self.values[i] = init

    def getWidth(self):
        return self.cols

    def getHeight(self):
        return self.rows
    
    def checkSize(self, other, msg):
        if self.cols != other.cols or self.rows != other.rows:
            raise msg + ":not the same size"
    def checkMul(self, other, msg):
        if self.cols != other.rows or self.rows != other.cols:
            raise msg + ":wrong size"
    def add(self, other):
        self.checkSize(other, "Matrix.add")
        n = Matrix(self.cols, self.rows)
        for i in range(len(self.values)):
            n.values[i] = self.values[i]+other.values[i]
        return n
    
    def sub(self, other):
        self.checkSize(other, "Matrix.sub")
        n = Matrix(self.cols, self.rows)
        for i in range(len(self.values)):
            n.values[i] = self.values[i]-other.values[i]
        return n
        
    def mulRow(self, other, row, col):
        sum=0
        for i in range(self.cols):
            sum+=self.get(row, i)*other.get(i, col)
        return sum
        
    def mul(self, other):
        self.checkMul(other, "Matrix.mul")
        n = Matrix(other.cols, self.rows)
        for row in range(n.rows):
            for col in range(n.cols):
                n.set(row, col, self.mulRow(other, row, col))
        return n
        
    def set(self, row, col, v):
        self.values[row*self.cols+col] = v

    def setxy(self, x, y, v):
        col = x
        row = self.rows - y - 1
        self.set(row, col, v)

    def getxy(self, x, y):
        col = x
        row = self.rows - y - 1
        return self.get(row, col)
        
    def get(self, row, col):
        return self.values[row*self.cols+col]

def clear():
    system("cls")
    # gotoxy(0,0)
    # for i in range(80 * 30):
    #     write('-')
    # gotoxy(0,0)

def paintHead(n):
    string = ""
    string += '|'
    # write('|')
    for i in range(n):
        # write('-')
        string += '-'
    # write('|\n')
    string += '|\n'
    return string

def repaint(m):
    t0 = clock()
    # gotoxy(0,0)
    string = ""
    string += paintHead(m.cols)
    for row in range(m.rows):
        # write('|')
        string += "|"
        for col in range(m.cols):
            x = m.get(row, col)
            # gotoxy(row+1, col+1)
            if x == 0:
                # write('x')
                string += " "
            else:
                # write(' ')
                string += x
        # write('|\n')
        string += "|\n"
    string += paintHead(m.cols)
    t1 = clock()
    clear()
    t2 = clock()
    write(string)
    t3 = clock()
    write("\n")
    printf("clearTime=%s, paintTime=%d, totalTime=%d\n", t2-t1,t3-t2, t2-t0)
    sleep(200)


def main():
    width = 70
    height = 20
    m = Matrix(width, height)
    snake = Snake()
    snake.world = m
    snake.draw()
    i = 0
    for i in range(100):
        # col = int(random() * (width-1))
        # row = int(random() * (height-1))
        # m.set(row, col, 1)
        snake.move()
        repaint(m)
        printf("snake.dir=%s", snake.dir)

if __name__ == '__main__':
    main()