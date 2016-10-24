import sys

def find_max_len(lines):
    max_len = 0;
    for line in lines:
        line = line.rstrip(" \\")
        l = len(line)
        if l > max_len:
            max_len = l
    return max_len

def pretty(text):
    text = text.replace("\r", "")
    lines = text.split("\n")
    max_len = find_max_len(lines)
    for line in lines:
        
        # comment
        if line.startswith("/*") or line.startswith(' *') or line.startswith(" */"):
            print (line)
            continue
            
        line = line.rstrip(" \\")
        line = line.ljust(max_len) + "\\"
        print(line)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print ("usage:%s filename" % sys.argv[0])
    else:
        text = open(sys.argv[1]).read()
        pretty(text)