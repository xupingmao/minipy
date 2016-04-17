from matrix import 1f

class ConsoleGraph:
    def __init__(self, width=80, height=20):
        self.width = width
        self.height = height
        self.matrix = Matrix(self.width, self.height, ' ')
    
    def draw(self, x, y):
        self.matrix.set(y,x,'*')
        #self.update()
    def dot(self, x, y):
        self.matrix.set(y,x,'*')
    def drawLine0(self, x0, y0, x1, y1):
        if x1 == x0: 
            if y0 > y1: y0,y1 = y1,y0
            for y in range(y0,y1+1):self.dot(x0,y)
        elif y0 == y1:
            for x in range(x0,x1+1):self.dot(x,y0)
        else: 
            rad = float(y1-y0) / (x1 - x0)
            for x in range(x0,x1+1):
                y = int(y0 + rad * (x - x0))
                self.dot(x,y)
        #self.update()
            
    def drawLine(self, x0,y0,x1,y1):
        if x0 < x1:self.drawLine0(x0,y0,x1,y1)
        else:self.drawLine0(x1,y1,x0,y0)
    
    def drawRect(self, x, y, w, h):
        pass
    
    def update(self):
        self.matrix.display()

if __name__ == '__main__':
    g = ConsoleGraph()
    g.draw(10,18)
    g.drawLine(0,0,10,10)
    g.drawLine(1,2,7,15)
    g.drawLine(0,10,10,10)
    g.drawLine(14,0,14,18)
    g.drawLine(0,17,60,17)
    g.drawLine(0,16,60,19)
    g.update()
    