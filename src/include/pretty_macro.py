import sys
import re
import time

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
    newlines = []
    for line in lines:
        # comment
        if line.startswith("/*") or line.startswith(' *') or line.startswith(" */"):
            newlines.append(line)
            continue
            
        line = line.rstrip(" \\")
        line = line.ljust(max_len) + "\\"
        newlines.append(line)
    lastline = newlines.pop()
    if lastline.strip() != "\\":
        newlines.append(lastline)
    new_text = "\n".join(newlines)
    new_text = re.sub(r"@Modified {[\d\- :]*}", "@Modified {%s}" % time.strftime('%Y-%m-%d %H:%M:%S'), new_text)
    print(new_text)
    return new_text + "\n"

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print ("美化宏函数")
        print ("usage:%s filename" % sys.argv[0])
    else:
        text = open(sys.argv[1]).read()
        # backup file
        with open(sys.argv[1]+".bak", "w") as fp:
            fp.write(text)
        new_text = pretty(text)
        with open(sys.argv[1], "w") as fp:
            fp.write(new_text)