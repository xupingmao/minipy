import os

def fixdir(path):
    for name in os.listdir(path):
        fixpath(os.path.join(path, name))

def fixfile(path):
	p = path.lower();
	if not p.endswith((".h", ".c", ".cpp")): return
    try:
        file = open(path, "r", encoding="utf8")
        content = file.read()
        file.close()
        if "\t" in content:
            content = content.replace("\t", "    ")
            file = open(path, "w", encoding="utf8")
            file.write(content)
            file.close()
            print("fix path: %s" % path)
    except:
        print("can not open as utf8: %s" % path)

def fixpath(path):
    if os.path.isdir(path):
        fixdir(path)
    else:
        fixfile(path)

if __name__ == "__main__":
    fixpath(".")