
cc = gcc
LOG_LEVEL?=2
MP_PROFILE?=0
RECORD_LAST_OP?=0
MP_LOG_CACHE?=0
ROOT=$(shell pwd)

minipy: src/*.c src/include/*.h
	$(cc) -DTM_USE_CACHE -DLOG_LEVEL=$(LOG_LEVEL)\
		-DMP_PROFILE=$(MP_PROFILE)\
		-DNDEBUG\
		-O2 -o minipy src/main.c -lm

.PHONY: clean test ctest

pack: src/*.c src/include/*.h pack/*.c
	$(cc) -DTM_USE_CACHE -DLOG_LEVEL=$(LOG_LEVEL)\
		-DMP_PROFILE=$(MP_PROFILE)\
		-DNDEBUG\
		-O2 -o pack_main pack/main.c -lm

o2:
	$(cc) -DTM_USE_CACHE -O2 -o minipy src/main.c -lm

o3:
	$(cc) -DTM_USE_CACHE -O3 -o minipy src/main.c -lm

minipyc_bin:
	python3 src/python/mp_encode.py -const_name minipyc_bin -c_code mp2c/minipyc.py > mp2c/gen/minipyc_bin.h
	python3 src/python/mp_encode.py -const_name mp2c_bin -c_code mp2c/mp2c.py > mp2c/gen/mp2c_bin.h

minipyc: minipyc_bin
	$(cc) -DTM_USE_CACHE -O2 -o minipyc mp2c/minipyc.c -lm

debug-gc:
	$(cc) -g -DLOG_LEVEL=5 -DMP_DEBUG -o minipy src/main.c -lm

debug:src/*.c src/include/*.h
	bash ./script/build-debug.sh

nogc:
	$(cc) -g -DGC_DESABLED -o minipy src/main.c -lm

check-mem: 
	$(cc) -DTM_CHECK_MEM -o minipy src/main.c -lm

test-reg-vs-stack: test/benchmark/reg_vs_stack/reg_vs_stack.c
	$(cc) -o build/reg_vs_stack test/benchmark/reg_vs_stack/reg_vs_stack.c -lm
	./build/reg_vs_stack

test-dict:
	make && ./minipy ./test/test_case/030_test_debug.py
	
test:
	make minipy
	./minipy ./test/test_main.py

test-tokenize:
	python3 ./test/test_tokenize.py

ctest:
	$(cc) -DTM_USE_CACHE -DLOG_LEVEL=$(LOG_LEVEL)\
		-DMP_PROFILE=$(MP_PROFILE)\
		-DRECORD_LAST_OP=$(RECORD_LAST_OP)\
		-o ctest test/ctest/ctest_main.c -lm -I src
	./ctest

tinypy:
	rm $(ROOT)/build/tinypy || echo "no tinypy"
	cd test/tinypy && python2 setup.py tinypy math random time
	mv test/tinypy/build/tinypy $(ROOT)/build/

micropython:
	python3 ./test/benchmark/build_micropython.py

benchmark:
	python3 ./test/benchmark/benchmark_main.py

clean : 
	rm -f minipy
