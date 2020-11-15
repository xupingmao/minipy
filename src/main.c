/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2020/10/26 22:54:31
 */
#include "vm.c"
#include "bin.c"

int main(int argc, char *argv[])
{
    /* start vm with bin */
    int ret = vm_init(argc, argv);
    if (ret != 0) { 
        return ret;
    }
    
    /* use first frame */
    int code = setjmp(tm->frames->buf);
    if (code == 0) {
        /* load python modules */
        load_boot_module("mp_init",     mp_init_bin);
        load_boot_module("mp_opcode",   mp_opcode_bin);
        load_boot_module("mp_tokenize", mp_tokenize_bin);
        load_boot_module("mp_parse",    mp_parse_bin);
        load_boot_module("mp_encode",   mp_encode_bin);
        load_boot_module("pyeval",      pyeval_bin);
        load_boot_module("repl",        repl_bin);
        obj_set_by_cstr(tm->builtins, "MP_USE_CACHE", number_obj(1));
        // call boot function
        call_mod_func("mp_init", "boot");
    } else if (code == 1){
        /* handle exceptions */
        mp_traceback();
    } else if (code == 2){
        /* minipy call exit() */
    }

    vm_destroy();
    return 0;
}

