# -*- coding:utf-8 -*-
# @author xupingmao
# @since 2022/02/13 17:51:33
# @modified 2022/02/13 18:00:35
# @filename build_micropython.py
import os

def check_and_move(target):
    if os.path.exists("micropython"):
        print("micropython可执行文件已经构建完成")
        os.rename("micropython", target)
        return True

def do_build(root, target):
    if not os.path.exists(root):
        print("目录不存在，准备clone...")
        os.system("git clone https://github.com/micropython/micropython --depth 1 %r" % root)

    print("找到MicroPython目录，准备构建...")
    os.chdir(root)
    os.chdir("./ports/unix")

    if check_and_move(target):
        return

    os.system("make submodules && make")
    
    check_and_move(target)

def main():
    cwd = os.getcwd()
    try:
        target = os.path.join("build", "micropython")
        target = os.path.abspath(target)
        do_build("./test/micropython", target)
    finally:
        os.chdir(cwd)

if __name__ == '__main__':
    main()