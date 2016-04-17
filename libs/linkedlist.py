
def print_dict(d):
    for k in d:
        print(k,d)

def Node(e, prev, next):
    obj = newobj()
    obj.prev = prev
    obj.next = next
    obj.value = e
    return obj

class LinkedList:
    def __init__(self):
        self.first = None
        self.last = None
        
    def poll(self):
        first = self.first
        if first == None:
            raise "likedlist is empty"
        self.first = first.next
        return first
        
    def put(self, e):
        if self.first == None:
            node = Node(e, None, None)
            self.first = node
            self.last = node
        else:
            node = Node(e, None, self.first)
            self.first = node
            
    def to_str(self):
        s = ''
        node = self.first
        while node != None:
            # print_dict(node)
            if node.next != None:
                s += str(node.value) + ","
            else:
                s += str(node.value)
            node = node.next
        return s
            
            
if __name__ == "__main__":
    list = LinkedList()
    list.put("test")
    list.put("hello")
    print(list.to_str())
    list.poll()
    print(list.to_str())
    list.poll()
    print(list.to_str())
    list.put(123)
    print(list.to_str())