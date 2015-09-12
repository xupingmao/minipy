from tmcode import *
from boot import *

def find_max(dict):
    max = 0
    for ins in dict:
        val = dict[ins]
        if val > max:
            max = val
            max_key = ins
    return max_key

def freq_analyze(code):
    read = 0
    size = len(code)
    freq_dict = {}
    while read < size:
        ins = ord(code[read])
        val = uncode16(code[read+1], code[read+2])
        read += 3
        if ins == NEW_STRING or ins == NEW_NUMBER:
            read += val
        if ins not in freq_dict:
            freq_dict[ins] = 0
        freq_dict[ins]+=1
    cnt = 0
    while len(freq_dict) > 0:
        cnt += 1
        max_ins = find_max(freq_dict)
        printf("%2s: %s %s\n", cnt, tmcode_str_dict[max_ins], freq_dict[max_ins])
        del freq_dict[max_ins]

if __name__ == '__main__':
    if len(ARGV) == 1:
        freq_analyze(load('core'))
    else:
        from encode import compilefile
        code = compilefile(ARGV[1])
        freq_analyze(code)