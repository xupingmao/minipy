/**
 * @file gc_bdwgc.c
 * @author 578749341@qq.com
 * @brief bdwgc的适配器
 * @version 0.1
 * @date 2024-04-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "deps/bdwgc/include/gc.h"

#define malloc GC_malloc
#define free GC_free