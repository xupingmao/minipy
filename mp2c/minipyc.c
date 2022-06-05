/**
 * minipy编译器
 * @author xupingmao
 * @since 2022/06/05 16:01:07
 * @modified 2022/06/05 17:02:26
 */
#include "../src/vm.c"
#include "./gen/minipyc_bin.h"
#include "./gen/mp2c_bin.h"

static void load_minipyc() {
    load_boot_module("mp2c", mp2c_bin);
    load_boot_module("minipyc",    minipyc_bin);
}

int main(int argc, char *argv[]) {
    /* start vm with bin */
    int ret = vm_init(argc, argv);
    if (ret != 0) { 
        return ret;
    }
    
    /* use first frame */
    int code = setjmp(tm->frames->buf);
    if (code == 0) {
        vm_load_py_modules();
        load_minipyc();
        obj_set_by_cstr(tm->builtins, "MP_USE_CACHE", number_obj(1));
        // call boot function
        vm_call_mod_func("minipyc", "boot");
    } else if (code == 1){
        /* handle exceptions */
        mp_traceback();
    } else if (code == 2){
        /* minipy call exit() */
    }

    vm_destroy();
    return 0;
}

