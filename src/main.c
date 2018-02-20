/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2018/02/19 16:59:47
 */
#include "vm.c"
#include "interp.c"
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
        /* init modules */
        time_mod_init();
        sys_mod_init();
        math_mod_init();
        os_mod_init();
        
        load_boot_module("init",   init_bin);
        load_boot_module("lex",    lex_bin);
        load_boot_module("parse",  parse_bin);
        load_boot_module("tmcode", tmcode_bin);
        load_boot_module("encode", encode_bin);
        load_boot_module("pyeval", pyeval_bin);
        load_boot_module("repl",   repl_bin);
        dict_set_by_str(tm->builtins, "TM_USE_CACHE", tm_number(1));
 
        if (tm_hasattr(tm->modules, "init")) {
            call_mod_func("init", "boot");
        } else if (tm_hasattr(tm->modules, "main")) {
            // adjust sys.argv
            call_mod_func("main", "_main");
        }
    } else if (code == 1){
        traceback();
    } else if (code == 2){
    }
    vm_destroy();
    return 0;
}

