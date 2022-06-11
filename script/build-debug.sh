make clean
gcc -DTM_USE_CACHE -DMP_PROFILE -DRECORD_LAST_OP\
	-g -o minipy src/main.c -lm
