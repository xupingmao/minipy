
cc = gcc

minipy : src/*.c src/include/*.h
	$(cc) -DTM_USE_CACHE -o minipy src/main.c -lm

.PHONY: clean test

o2:
	$(cc) -DTM_USE_CACHE -o minipy -O2 -lm src/main.c

debug:
	$(cc) -DTM_CHECK_MEM -o minipy -g -lm src/main.c

check_mem: 
	$(cc) -DTM_CHECK_MEM -o minipy -lm src/main.c
	
test:
	./minipy ./test/test-main.py

clean : 
	rm minipy
	
	
