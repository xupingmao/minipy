
cc = gcc

minipy: src/*.c src/include/*.h
	$(cc) -DTM_USE_CACHE -o minipy src/main.c -lm

.PHONY: clean test

o2:
	$(cc) -DTM_USE_CACHE -o minipy -O2 -lm src/main.c

o3:
	$(cc) -DTM_USE_CACHE -o minipy -O3 -lm src/main.c

debug:
	$(cc) -g -o minipy src/main.c -lm

check_mem: 
	$(cc) -DTM_CHECK_MEM -o minipy src/main.c -lm
	
test:
	./minipy ./test/test_main.py

clean : 
	rm minipy
	
	
