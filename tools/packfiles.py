import os
import sys

def readfile(path):
    content = None
    try:
        fp = open(path, encoding="utf-8")
        content = fp.read()
        fp.close()
    except Exception as e:
        fp = open(path, encoding="gbk")
        content = fp.read()
        fp.close()
    finally:
        return content

def getfname(line):
    fname = line.replace('#include', '')
    if '<' in fname:
        return None
    fname = fname.replace('"', '')
    fname = fname.replace(' ', '')
    return fname


def packfiles(filename, out = None, packed_files = None):
    if out is None:
        out = []
    if packed_files is None:
        packed_files = {}

    if not os.path.exists(filename):
        raise Exception("file %s not exists" % filename)

    dirname = os.path.dirname(filename)
    basename = os.path.basename(filename)

    if basename in packed_files:
        return

    packed_files[basename] = True

    content = readfile(filename)
    for line in content.split('\n'):
        if line is None:
            continue
        if line.lstrip().startswith('#include'):
            fname = getfname(line)
            if fname is None:
                out.append(line)
            else:
                fname = os.path.join(dirname, fname)
                packfiles(fname, out, packed_files)
        else:
            out.append(line)
    return "\n".join(out)

if __name__ == '__main__':
    # print(os.getcwd())
    content = packfiles(sys.argv[1])
    print(content)