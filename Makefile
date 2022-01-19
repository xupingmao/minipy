
cc = gcc
LOG_LEVEL?=2
MP_PROFILE?=0

minipy: src/*.c src/include/*.h
	$(cc) -DTM_USE_CACHE -DLOG_LEVEL=$(LOG_LEVEL)\
		-DMP_PROFILE=$(MP_PROFILE)\
		-o minipy src/main.c -lm

.PHONY: clean test

o2:
	$(cc) -DTM_USE_CACHE -o minipy -O2 -lm src/main.c

o3:
	$(cc) -DTM_USE_CACHE -o minipy -O3 -lm src/main.c

debuggc:
	$(cc) -g -DLOG_LEVEL=5 -DMP_DEBUG -o minipy src/main.c -lm

debug:
	$(cc) -g -DMP_DEBUG -o minipy src/main.c -lm

nogc:
	$(cc) -g -DGC_DESABLED -o minipy src/main.c -lm

check_mem: 
	$(cc) -DTM_CHECK_MEM -o minipy src/main.c -lm
	
test:
	./minipy ./test/test_main.py

clean : 
	rm minipy
	
