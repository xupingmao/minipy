#include "../src/vm.c"
#include "main.gen.c"

void load_pack_main() {
    load_boot_module("main", main_bin);
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
        obj_set_by_cstr(tm->builtins, "MP_USE_CACHE", number_obj(1));
        vm_load_py_modules();
        load_pack_main();
    } else if (code == 1){
        /* handle exceptions */
        mp_traceback();
    } else if (code == 2){
        /* minipy call exit() */
    }

    vm_destroy();
    return 0;
}

