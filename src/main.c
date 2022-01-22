/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2022/01/15 13:07:11
 */
#include "vm.c"

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
        obj_set_by_cstr(tm->builtins, "MP_USE_CACHE", number_obj(1));
        // call boot function
        vm_call_mod_func("mp_init", "boot");
    } else if (code == 1){
        /* handle exceptions */
        mp_traceback();
    } else if (code == 2){
        /* minipy call exit() */
    }

    vm_destroy();
    return 0;
}

