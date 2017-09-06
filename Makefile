
cc = gcc

minipy : src/*.c src/include/*.h
	$(cc) -DTM_USE_CACHE -o minipy src/main.c -lm

.PHONY: clean test

check_mem: 
	$(cc) -DTM_CHECK_MEM -o minipy src/main.c -lm
	
test:
	./minipy ./test/test-main.py

clean : 
	rm minipy
	
	
