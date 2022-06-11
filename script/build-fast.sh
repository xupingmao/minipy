gcc -DTM_USE_CACHE -DNDEBUG\
	-DFAST_GET -DFAST_LOAD_GLOBAL\
	-O2\
	-o build/minipy-fast src/main.c -lm
