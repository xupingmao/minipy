/*
 * @Author: xupingmao 578749341@qq.com
 * @Date: 2024-04-14 12:34:32
 * @LastEditors: xupingmao 578749341@qq.com
 * @LastEditTime: 2024-04-14 13:17:57
 * @FilePath: /minipy/src/gc_bdwgc.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
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
#include "deps/bdwgc/extra/gc.c"

#define malloc GC_malloc
#define free GC_free