
cc = gcc
LOG_LEVEL?=2
MP_PROFILE?=0
ROOT=$(shell pwd)

minipy: src/*.c src/include/*.h
	$(cc) -DTM_USE_CACHE -DLOG_LEVEL=$(LOG_LEVEL)\
		-DMP_PROFILE=$(MP_PROFILE)\
		-o minipy src/main.c -lm

.PHONY: clean test

o2:
	$(cc) -DTM_USE_CACHE -O2 -o minipy src/main.c -lm

o3:
	$(cc) -DTM_USE_CACHE -O3 -o minipy src/main.c -lm

debug-gc:
	$(cc) -g -DLOG_LEVEL=5 -DMP_DEBUG -o minipy src/main.c -lm

debug:
	$(cc) -g -DMP_DEBUG -o minipy src/main.c -lm

nogc:
	$(cc) -g -DGC_DESABLED -o minipy src/main.c -lm

check-mem: 
	$(cc) -DTM_CHECK_MEM -o minipy src/main.c -lm

test-reg-vs-stack:
	$(cc) -o build/reg_vs_stack test/reg_vs_stack/reg_vs_stack.c -lm
	./build/reg_vs_stack

test-dict:
	make && ./minipy ./test/test_case/030_test_debug.py
	
test:
	./minipy ./test/test_main.py

tinypy:
	rm $(ROOT)/build/tinypy || echo "no tinypy"
	cd test/tinypy && python2 setup.py tinypy math random time
	mv test/tinypy/build/tinypy $(ROOT)/build/

benchmark:
	python3 ./test/benchmark/benchmark_main.py

clean : 
	rm minipy
