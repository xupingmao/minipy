/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2020/10/23 00:37:03
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
        dict_set_by_str(tm->builtins, "TM_USE_CACHE", number_obj(1));
 
        if (mp_hasattr(tm->modules, "mp_init")) {
            call_mod_func("mp_init", "boot");
        } else if (mp_hasattr(tm->modules, "main")) {
            // adjust sys.argv
            call_mod_func("main", "_main");
        } else {
            mp_raise("no entry found");
        }
    } else if (code == 1){
        /* handle exceptions */
        traceback();
    } else if (code == 2){
        /* minipy call exit() */
    }

    vm_destroy();
    return 0;
}

