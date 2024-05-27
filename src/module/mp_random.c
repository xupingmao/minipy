#include <stdlib.h>
#include <time.h>

static MpObj random_randint() {
	int start = mp_take_int_arg("random.randint");
	int stop  = mp_take_int_arg("random.randint");

	if (start > stop) {
		mp_raise("random_randint: invalid range(%d, %d)", start, stop);
	}

	int gap = stop - start + 1;
	
	// rand() 生成 [0, RAND_MAX] 范围内的随机数
	int value = rand() % gap;
	return mp_number(start + value);
}

void mp_random_init() {
	int t = time(NULL) + clock();
	// 初始化随机种子
	srand((unsigned int)t);
	
    MpObj module = mp_new_native_module("random");

	// functions
    MpModule_RegFunc(module, "randint",  random_randint);
	// attributes
    MpModule_RegAttr(module, "RAND_MAX", mp_number(RAND_MAX));
}

