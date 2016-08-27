#include "../include/tm.h"
#include <math.h>

/**
 * math module for minipy
 * @since 2016-08-27
 * @author xupingmao
 * @email 578749341@qq.com
 */

Object math_sin() {
    double angle = arg_take_double("math.sin");
    return tm_number(sin(angle));
}

Object math_cos() {
    double angle = arg_take_double("math.cos");
    return tm_number(cos(angle));
}

Object math_tan() {
    double angle = arg_take_double("math.tan");
    return tm_number(tan(angle));
}

Object math_acos() {
    double angle = arg_take_double("math.acos");
    return tm_number(acos(angle));
}

Object math_asin() {
    double angle = arg_take_double("math.asin");
    return tm_number(asin(angle));
}

Object math_atan() {
    double angle = arg_take_double("math.atan");
    return tm_number(atan(angle));
}

Object math_atan2() {
    double y = arg_take_double("math.atan2");
    double x = arg_take_double("math.atan2");
    return tm_number(atan2(y,x));
}

/**
 * init math module
 * @since 2016-08-27
 */
void math_mod_init() {
    Object math = dict_new();

    dict_set_by_str(math, "PI", tm_number(3.141592653589793));
    reg_mod_func(math, "sin", math_sin);
    reg_mod_func(math, "cos", math_cos);
    reg_mod_func(math, "tan", math_tan);
    reg_mod_func(math, "acos", math_acos);
    reg_mod_func(math, "asin", math_asin);
    reg_mod_func(math, "atan", math_atan);
    reg_mod_func(math, "atan2", math_atan2);

    dict_set_by_str(tm->modules, "math", math);
}