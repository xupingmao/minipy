
cc = gcc

minipy : src/*.c src/include/*.h
	$(cc) -DTM_USE_CACHE -o minipy src/main.c -lm

.PHONY: clean test
	
test:
	./minipy ./test/test-main.py

clean : 
	rm minipy
	
	
