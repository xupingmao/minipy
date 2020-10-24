#include "../include/mp.h"
#include <math.h>

/**
 * math module for minipy
 * @since 2016-08-27
 * @author xupingmao
 * @email 578749341@qq.com
 */

 
static
Object math_abs() {
    double angle = arg_take_double("math.abs");
    if (angle < 0) { angle = -angle; }
    return number_obj(angle);
}

static
Object math_acos() {
    double angle = arg_take_double("math.acos");
    return number_obj(acos(angle));
}

static
Object math_asin() {
    double angle = arg_take_double("math.asin");
    return number_obj(asin(angle));
}

static
Object math_atan() {
    double angle = arg_take_double("math.atan");
    return number_obj(atan(angle));
}

static
Object math_atan2() {
    double y = arg_take_double("math.atan2");
    double x = arg_take_double("math.atan2");
    return number_obj(atan2(y,x));
}

static
Object math_ceil() {
    double value = arg_take_double("math.ceil");
    return number_obj(ceil(value));
}

static
Object math_cos() {
    double angle = arg_take_double("math.cos");
    return number_obj(cos(angle));
}

static
Object math_cosh() {
    double value = arg_take_double("math.cosh");
    return number_obj(cosh(value));
}

static
Object math_sin() {
    double angle = arg_take_double("math.sin");
    return number_obj(sin(angle));
}

static
Object math_tan() {
    double angle = arg_take_double("math.tan");
    return number_obj(tan(angle));
}

/**
 * init math module
 * @since 2016-08-27
 */
void math_mod_init() {
    Object math = dict_new();

    dict_set_by_str(math, "PI", number_obj(3.141592653589793));
    reg_mod_func(math, "abs", math_abs);
    reg_mod_func(math, "acos", math_acos);
    reg_mod_func(math, "asin", math_asin);
    reg_mod_func(math, "atan", math_atan);
    reg_mod_func(math, "atan2", math_atan2);
    
    reg_mod_func(math, "ceil", math_ceil);
    reg_mod_func(math, "cos", math_cos);
    reg_mod_func(math, "cosh", math_cosh);
    
    reg_mod_func(math, "sin", math_sin);
    reg_mod_func(math, "tan", math_tan);

    dict_set_by_str(tm->modules, "math", math);
}