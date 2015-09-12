info = getMallocInfo()
def sort(dict):
    keys = dict.keys()
    values = dict.values()
    i = 0
    while i < len(values):
        j = i
        tar = values[i]
        tarK = keys[i]
        while j > 0 and tar > values[j - 1]:
            values[j] = values[j-1]
            keys[j] = keys[j-1]
            j -= 1
        values[j] = tar
        keys[j] = tarK
        i += 1
    return keys, values

# print(info)
# info = {10:10, 20:120}
keys, values = sort(info)

def min(a,b):
    if a < b:
        return a
    return b

i = 0
while i < min(100, len(keys)):
    print(keys[i], values[i])
    i += 1
    