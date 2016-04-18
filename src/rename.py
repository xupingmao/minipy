# run with python3
import os, sys

def copy(src, des):
    fp = open(des, "w")
    for line in open(src):
        fp.write(line)
    fp.close()

def bakupdir(dirname, file_exts, bakdir):
    absbakdir = os.path.join(dirname, bakdir)
    if not os.path.exists(absbakdir):
        os.mkdir(absbakdir)
    for fname in os.listdir(dirname):
        abspath = os.path.join(dirname, fname)
        if os.path.isfile(abspath) and fname.endswith(file_exts):
            copy(abspath, os.path.join(dirname, bakdir, fname))

def rename(dirname, changes, file_exts = (".h", ".c"), bakdir = "bak"):
    # bakupdir(dirname, file_exts, bakdir)
    oldname, newname = changes
    for fname in os.listdir(dirname):
        abspath = os.path.join(dirname, fname)
        if os.path.isfile(abspath) and fname.endswith(file_exts):
            # print("load file %s" % abspath)
            text = open(abspath).read()
            if oldname not in text:
                continue
            print("replace file %s" % abspath)
            newtext = text.replace(oldname, newname)
            open(abspath, "w").write(newtext)

def do_replace(oldname, newname):
    changes = oldname, newname
    rename(".", changes)
    rename("include", changes)
    #rename("lib", changes)

def clean_baks():
    print("<function clean_baks> not implemented")
    
if __name__ == "__main__":
    argc = len(sys.argv)
    argv = sys.argv
    if argc == 2 and argv[1] == '-clean':
        clean_baks()
    elif argc == 3:
        do_replace(argv[1], argv[2])
    else:
        print("usage: %s oldname newname" % sys.argv[0])
        print("    or %s -clean" % argv[0])
    

