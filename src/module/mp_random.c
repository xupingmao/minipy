#include <stdlib.h>
#include <time.h>

static MpObj random_randint() {
	int start = arg_take_int("random.randint");
	int stop  = arg_take_int("random.randint");

	if (start > stop) {
		mp_raise("random_randint: invalid range(%d, %d)", start, stop);
	}

	int gap = stop - start + 1;
	
	// rand() 生成 [0, RAND_MAX] 范围内的随机数
	int value = rand() % gap;
	return number_obj(start + value);
}

void init_mod_for_random() {
	int t = time(NULL) + clock();
	// 初始化随机种子
	srand((unsigned int)t);
	

    MpObj module = dict_new();
    reg_mod("random", module);

    reg_mod_func(module, "randint",  random_randint);

    reg_mod_attr(module, "RAND_MAX", number_obj(RAND_MAX));
}

