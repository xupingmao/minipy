/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2020/10/02 10:28:57
 */
#include "vm.c"
#include "execute.c"
#include "bin.c"

int main(int argc, char *argv[])
{
    /* start vm with bin */
    int ret = vm_init(argc, argv);
    if (ret != 0) { 
        return ret;
    }
    // tm->code = bin;

    /* use first frame */
    int code = setjmp(tm->frames->buf);
    if (code == 0) {
        /* init c modules */
        time_mod_init();
        sys_mod_init();
        math_mod_init();
        os_mod_init();
        
        /* load python modules */
        load_boot_module("mp_init",     mp_init_bin);
        load_boot_module("mp_opcode",   mp_opcode_bin);
        load_boot_module("mp_tokenize", mp_tokenize_bin);
        load_boot_module("mp_parse",    mp_parse_bin);
        load_boot_module("mp_encode",   mp_encode_bin);
        load_boot_module("pyeval",      pyeval_bin);
        load_boot_module("repl",        repl_bin);
        dict_set_by_str(tm->builtins, "TM_USE_CACHE", tm_number(1));
 
        if (tm_hasattr(tm->modules, "mp_init")) {
            call_mod_func("mp_init", "boot");
        } else if (tm_hasattr(tm->modules, "main")) {
            // adjust sys.argv
            call_mod_func("main", "_main");
        } else {
            tm_raise("no entry found");
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

