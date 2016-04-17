# sort algorithms

def insert_sort(values):
    for i in range(len(values)):
        j = i
        tar = values[i]
        while j > 0 and tar < values[j - 1]:
            values[j] = values[j-1]
            j -= 1
        values[j] = tar
    return values