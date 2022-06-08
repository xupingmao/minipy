/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2022/06/05 23:52:01
 */
#include "vm.c"

static void test_code_queue_1() {
	CodeQueue queue;
	MpCodeCache cache;

	CodeQueue_Init(&queue);

	cache.op = 1;
	CodeQueue_Append(&queue, cache);
	cache.op = 2;
	CodeQueue_Append(&queue, cache);
	cache.op = 3;
	CodeQueue_Append(&queue, cache);

	assert(queue.size == 3);
	assert(queue.start == 0);
	assert(queue.data[queue.start].op == 1);
}

static void test_code_queue_2() {
	CodeQueue queue;
	MpCodeCache cache;

	CodeQueue_Init(&queue);

	cache.op = 1;
	CodeQueue_Append(&queue, cache);
	cache.op = 2;
	CodeQueue_Append(&queue, cache);
	cache.op = 3;
	CodeQueue_Append(&queue, cache);
	cache.op = 4;
	CodeQueue_Append(&queue, cache);
	cache.op = 5;
	CodeQueue_Append(&queue, cache);
	cache.op = 6;
	CodeQueue_Append(&queue, cache);

	assert(queue.size == 5);
	assert(queue.start == 1);
	assert(queue.data[queue.start].op == 2);
}


int main(int argc, char *argv[]) {
	test_code_queue_1();
	printf("test_code_queue_1 SUCCESS\n");

	test_code_queue_2();
	printf("test_code_queue_2 SUCCESS\n");
	return 0;
}