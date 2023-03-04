# encoding=utf-8
import sys
import os
sys.path.append("src/python")
import mp_encode

def compile_and_save(fpath, target):
    print("compile %s to %s" % (fpath, target))
    c = mp_encode.Compiler()
    code = c.compile_to_c_code(fpath)
    fp = open(target, "w+")
    fp.write(code)
    fp.close()

def do_build_core():
    names = ["mp_init", "mp_tokenize", "mp_parse", "mp_encode", "mp_opcode", "pyeval", "repl"]
    for name in names:
        source = "./src/python/%s.py" % name
        target = "./src/gen/%s.gen.c" % name
        compile_and_save(source, target)

def do_build():
    print("current path: %s" % os.getcwd())
    print("python interpreter: %s" % sys.executable)
    do_build_core()
    print("compile python files done!")
    os.system("make clean && make")

def do_build_user():
    names = ["main"]
    for name in names:
        source = "./pack/%s.py" % name
        target = "./pack/%s.gen.c" % name
        compile_and_save(source, target)

def build_pack():
    print("current path: %s" % os.getcwd())
    print("python interpreter: %s" % sys.executable)
    do_build_core()
    do_build_user()
    print("compile python files done!")
    os.system("make pack")

def main():
    arg1 = ""
    if len(sys.argv) >= 2:
        arg1 = sys.argv[1]
    
    if arg1 == "pack":
        build_pack()
        return
    
    do_build()

if __name__ == "__main__":
    main()
