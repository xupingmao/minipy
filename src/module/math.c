#include "../include/mp.h"
#include <math.h>

/**
 * math module for minipy
 * @since 2016-08-27
 * @author xupingmao
 * @email 578749341@qq.com
 */

 
static
MpObj math_abs() {
    double angle = mp_take_double_arg("math.abs");
    if (angle < 0) { angle = -angle; }
    return number_obj(angle);
}

static
MpObj math_acos() {
    double angle = mp_take_double_arg("math.acos");
    return number_obj(acos(angle));
}

static
MpObj math_asin() {
    double angle = mp_take_double_arg("math.asin");
    return number_obj(asin(angle));
}

static
MpObj math_atan() {
    double angle = mp_take_double_arg("math.atan");
    return number_obj(atan(angle));
}

static
MpObj math_atan2() {
    double y = mp_take_double_arg("math.atan2");
    double x = mp_take_double_arg("math.atan2");
    return number_obj(atan2(y,x));
}

static
MpObj math_ceil() {
    double value = mp_take_double_arg("math.ceil");
    return number_obj(ceil(value));
}

static
MpObj math_cos() {
    double angle = mp_take_double_arg("math.cos");
    return number_obj(cos(angle));
}

static
MpObj math_cosh() {
    double value = mp_take_double_arg("math.cosh");
    return number_obj(cosh(value));
}

static
MpObj math_sin() {
    double angle = mp_take_double_arg("math.sin");
    return number_obj(sin(angle));
}

static
MpObj math_tan() {
    double angle = mp_take_double_arg("math.tan");
    return number_obj(tan(angle));
}

static MpObj math_pow() {
    double base = mp_take_double_arg("math.pow");
    double y = mp_take_double_arg("math.pow");
    return number_obj(pow(base, y));
}

/**
 * init math module
 * @since 2016-08-27
 */
void mp_math_init() {
    MpObj math = mp_new_native_module("math");

    obj_set_by_cstr(math, "PI", number_obj(3.141592653589793));
    MpModule_RegFunc(math, "abs", math_abs);
    MpModule_RegFunc(math, "acos", math_acos);
    MpModule_RegFunc(math, "asin", math_asin);
    MpModule_RegFunc(math, "atan", math_atan);
    MpModule_RegFunc(math, "atan2", math_atan2);
    
    MpModule_RegFunc(math, "ceil", math_ceil);
    MpModule_RegFunc(math, "cos", math_cos);
    MpModule_RegFunc(math, "cosh", math_cosh);
    
    MpModule_RegFunc(math, "sin", math_sin);
    MpModule_RegFunc(math, "tan", math_tan);
    MpModule_RegFunc(math, "pow", math_pow);
}