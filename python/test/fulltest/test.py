
'''
# debugger interface. fidx is the frame index.
def __debug__(fidx):
    info = vmopt("frameinfo", fidx)
    #for key in info: print(key, info[key])
    lcnt = info.maxlocals
    scnt = info.stacksize
    print(info.fname, info.lineno)
    for i in range(lcnt):
        print("#"+str(i), vmopt("frame.local", fidx, i))
    for i in range(scnt):
        print(":"+str(i), vmopt("frame.stack", fidx, i))
'''

def run():
    list = listdir('.')
    for file in list:
        if file == "test.py":
            continue
        try:
            execfile(file) ##.debugger
            print("PASS", file)
        except Exception as e:
            print("FAIL", file, "\n  ",e)
  
run()