# encoding=utf-8

import re


class GcDebugLogAnalyzer:

    def handle(self, log_file="minipy.log"):
        malloc_dict = {}
        track_set = set()
        str_dict = {}

        malloc_pattern = re.compile(r"malloc:([0-9a-z]+),scene:([^,]+),size:([0-9]+)")
        free_pattern = re.compile(r"free:([0-9a-z]+),size:([0-9]+)")
        track_pattern = re.compile(r"track:([0-9a-z]+)")
        str_pattern = re.compile(r"str_ptr:([0-9a-z]+),str_value:([^\n+])")

        malloc_count = 0
        free_count = 0
        free_miss_count = 0
        track_miss_count = 0

        with open(log_file, "rb+") as fp:
            while True:
                line = fp.readline()
                if len(line) == 0:
                    break

                line = line.decode("utf-8", errors="ignore")
                line = line.strip()
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

                result = track_pattern.search(line)
                if result:
                    pointer = result.group(1)
                    track_set.add(pointer)

                result = str_pattern.search(line)
                if result:
                    pointer = result.group(1)
                    value = result.group(2)
                    str_dict[pointer] = value
            
        for pointer in malloc_dict:
            scene = malloc_dict[pointer]
            print(f"miss free {pointer} scene:{scene}")
            free_miss_count += 1
            if pointer not in str_dict:
                value = str_dict.get(pointer)
                if value:
                    print(f"miss free str {value}")

        for pointer in malloc_dict:
            scene = malloc_dict[pointer]
            if pointer not in track_set:
                track_miss_count+=1
                print(f"miss track {pointer} scene:{scene}")
        
        if free_miss_count == 0:
            print("no free miss")
        
        if track_miss_count == 0:
            print("no track miss")

        print(f"malloc_count={malloc_count}, free_count={free_count}, len(str_dict)={len(str_dict)}")


if __name__ == "__main__":
    anayzer = GcDebugLogAnalyzer()
    anayzer.handle()

