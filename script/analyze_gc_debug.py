# encoding=utf-8

import re


class GcDebugLogAnalyzer:

    def handle(self, log_file="minipy.log"):
        malloc_dict = {}

        malloc_pattern = re.compile(r"malloc:([0-9a-z]+),scene:([^,]+),size:([0-9]+)")
        free_pattern = re.compile(r"free:([0-9a-z]+),size:([0-9]+)")

        malloc_count = 0
        free_count = 0

        with open(log_file, "r+") as fp:
            while True:
                line = fp.readline()
                line = line.strip()
                if line == "":
                    break

                result = malloc_pattern.search(line)
                if result:
                    pointer = result.group(1)
                    scene = result.group(2)
                    malloc_dict[pointer] = scene
                    malloc_count+=1
                
                result = free_pattern.search(line)
                if result:
                    pointer = result.group(1)
                    malloc_dict.pop(pointer, None)
                    free_count+=1
            
        for pointer in malloc_dict:
            scene = malloc_dict[pointer]
            print(f"free leaks {pointer} scene:{scene}")

        print(f"malloc_count={malloc_count}, free_count={free_count}")


if __name__ == "__main__":
    anayzer = GcDebugLogAnalyzer()
    anayzer.handle()

