python3 src/python/mp_encode.py -const_name minipyc_bin -c_code mp2c/minipyc.py > mp2c/gen/minipyc_bin.h
python3 src/python/mp_encode.py -const_name mp2c_bin -c_code mp2c/mp2c.py > mp2c/gen/mp2c_bin.h

gcc -DTM_USE_CACHE -O2 -o build/minipy-aot mp2c/minipyc.c -lm