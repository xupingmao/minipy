import os, sys

def count(fname, word):
    text = open(fname).read()
    cnt = text.count(word)
    if cnt > 0:
        print("%-30s:%s" % (fname, cnt))
    return cnt
    
def count_dir(dir, word, filter):
    cnt = 0
    for fname in os.listdir(dir):
        if filter and not fname.endswith(filter):
            continue
        abspath = os.path.join(dir, fname)
        cnt += count(abspath, word)
    return cnt

def count_dir_list(dir_list, word, filter = ('.h', '.c')):
    cnt = 0
    for dir in dir_list:
        cnt += count_dir(dir, word, filter)
    return cnt

if __name__ == "__main__":
    cnt = count_dir_list(['.', 'include'], sys.argv[1])
    print(cnt)