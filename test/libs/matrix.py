def puts(v):
    sys.stdout.write(str(v))
if str(1.0) != '1.0':puts = printf
else: import sys

def make_list(cnt):
    list = []
    for x in range(cnt):
        list.append(0)
    return list

class Matrix:
    # m  [x  , y] 
    #    row, col
    # just like array in c lang
    def __init__(self, width, height, init = 0):
        self.width = width
        self.height = height
        self.consoleWidth = 80
        self.values = make_list(width * height)
        for i in range(len(self.values)):
            self.values[i] = init
    
    def checkSize(self, other, msg):
        if self.width != other.width or self.height != other.height:
            raise msg + ":not the same size"
    def checkMul(self, other, msg):
        if self.width != other.height or self.height != other.width:
            raise msg + ":wrong size"
    def add(self, other):
        self.checkSize(other, "Matrix.add")
        n = Matrix(self.width, self.height)
        for i in range(len(self.values)):
            n.values[i] = self.values[i]+other.values[i]
        return n
    
    def sub(self, other):
        self.checkSize(other, "Matrix.sub")
        n = Matrix(self.width, self.height)
        for i in range(len(self.values)):
            n.values[i] = self.values[i]-other.values[i]
        return n
    def mulRow(self, other, row, col):
        sum=0
        for i in range(self.width):
            sum+=self.get(row, i)*other.get(i, col)
        return sum
    def mul(self, other):
        self.checkMul(other, "Matrix.mul")
        n = Matrix(other.width, self.height)
        for row in range(n.height):
            for col in range(n.width):
                n.set(row, col, self.mulRow(other, row, col))
        return n
    def set(self, row, col, v):
        self.values[row*self.width+col] = v
    def get(self, row, col):
        return self.values[row*self.width+col]
    def display(self, printLine = 0):
        for row in range(self.height):
            for col in range(self.width):
                puts(self.get(row,col))
            if self.width != self.consoleWidth:print("")
        if printLine:print("")
if __name__ == '__main__':
    x = Matrix(10,10)
    y = Matrix(10,10)
    y.set(0,9,10)
    y.set(2,8,7)
    x.display(1)
    y.display(1)
    x.add(y).display()
    x = Matrix(5,2)
    y = Matrix(2,5)
    x.set(0,1,10)
    x.set(0,3,12)
    y.set(3,1,4)
    x.display(1);y.display(1)
    x.mul(y).display(1)