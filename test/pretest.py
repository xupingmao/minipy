from boot import *

std_boot = '../boot.py'
cur_boot = 'boot.py'

def sync(std, current):
    if mtime(std) > mtime(current):
        copy(std, current)

sync(std_boot, cur_boot)
sync("../tm.exe", "tm.exe")

    