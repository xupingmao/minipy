#include "../include/mp.h"
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

static void os_listdir_impl(MpObj list, MpObj path);

#if _WIN32
    #include "mp_os_win32.c"
#else
    #include "mp_os_posix.c"
#endif

MpObj os_getcwd() {
    const char* sz_func = "getcwd";
    char buf[1025];
    char* r = getcwd(buf, 1024);
    if (r == NULL) {
        char *msg;
        switch (errno) {
            case EINVAL: msg = "The size of argument is 0";break;
            case ERANGE: msg = "The size argument is greater than 0, but is smaller than the length of the pathname +1.";break;
            case EACCES: msg = "Read or search permission was denied for a component of the pathname.";break;
            case ENOMEM: msg = "Insufficient storage space is available.";break;
        }
        mp_raise("%s: error -- %s", sz_func, msg);
    }
    return string_new(buf);
}


MpObj os_chdir() {
    const char* sz_func = "chdir";
    char *path = mp_take_cstr_arg(sz_func);
    int r = chdir(path);
    if (r != 0) {
        mp_raise("%s: -- fatal error, can not chdir(\"%s\")", sz_func, path);
    } 
    return NONE_OBJECT;
}

MpObj os_listdir() {
    MpObj list = list_new(10);
    MpObj path = mp_take_str_obj_arg("listdir");
    os_listdir_impl(list, path);
    return list;
}

MpObj os_stat(){
    const char *s = mp_take_cstr_arg("stat");
    struct stat stbuf;
    if (!stat(s,&stbuf)) { 
        MpObj st = dict_new();
        obj_set_by_cstr(st, "st_mtime", mp_number(stbuf.st_mtime));
        obj_set_by_cstr(st, "st_atime", mp_number(stbuf.st_atime));
        obj_set_by_cstr(st, "st_ctime", mp_number(stbuf.st_ctime));
        obj_set_by_cstr(st, "st_size" , mp_number(stbuf.st_size));
        obj_set_by_cstr(st, "st_mode",  mp_number(stbuf.st_mode));
        obj_set_by_cstr(st, "st_nlink", mp_number(stbuf.st_nlink));
        obj_set_by_cstr(st, "st_dev",   mp_number(stbuf.st_dev));
        obj_set_by_cstr(st, "st_ino",   mp_number(stbuf.st_ino));
        obj_set_by_cstr(st, "st_uid",   mp_number(stbuf.st_uid));
        obj_set_by_cstr(st, "st_gid",   mp_number(stbuf.st_gid));
        return st;
    }
    mp_raise("stat(%s): file not exists or accessable.",s);
    return NONE_OBJECT;
}

MpObj os_exists(){
    MpObj _fname = mp_take_str_obj_arg("exists");
    char* fname = GET_CSTR(_fname);
    if (access(fname, F_OK) == 0) {
        return tm->_TRUE;
    }
    return tm->_FALSE;
}

MpObj os_path_dirname() {
    MpObj fpath = mp_take_str_obj_arg("os.path.dirname");
    mp_assert_type(fpath, TYPE_STR, "os.path.dirname");

    char* fpath_cstr = GET_CSTR(fpath);
    char* end_char = strrchr(fpath_cstr, '/');

    if (end_char) {
        int end = end_char - fpath_cstr;
        return string_substring(GET_STR_OBJ(fpath), 0, end);
    }

    end_char = strrchr(fpath_cstr, '\\');
    if (end_char) {
        int end = end_char - fpath_cstr;
        return string_substring(GET_STR_OBJ(fpath), 0, end);
    }

    return string_static("");
}

static MpObj os_path_join0(MpObj dirname, MpObj fname) {
    mp_assert_type(dirname, TYPE_STR, "os_path_join");
    mp_assert_type(dirname, TYPE_STR, "os_path_join");

    MpObj sep  = string_chr('/');
    MpObj temp = obj_add(dirname, sep);
    return obj_add(temp, fname);
}

MpObj os_get_name() {
#ifdef _WIN32
    return string_from_cstr("nt");
#else
    return string_from_cstr("posix");
#endif
}

static MpObj os_system() {
    char* command = mp_take_cstr_arg("os.system");
    int ret_code = system(command);
    return mp_number(ret_code);
}

static MpObj os_mkdir() {
    char* dirname = mp_take_cstr_arg("os.mkdir");
    int status = mkdir(dirname);
    if (status == 0) {
        return NONE_OBJECT;
    } else {
        mp_raise("mkdir failed");
    }
    return NONE_OBJECT;
}

/**
 * @brief OS模块
 */
void mp_os_init() {
    MpObj os_mod = mp_new_native_module("os");
    MpObj os_path_mod = mp_new_native_module("os.path");

    MpModule_RegFunc(os_mod, "getcwd",  os_getcwd);
    MpModule_RegFunc(os_mod, "listdir", os_listdir);
    MpModule_RegFunc(os_mod, "chdir",   os_chdir);
    MpModule_RegFunc(os_mod, "stat",    os_stat);
    MpModule_RegFunc(os_mod, "exists",  os_exists);
    MpModule_RegFunc(os_mod, "system",  os_system);
    MpModule_RegFunc(os_mod, "mkdir", os_mkdir);

    // 注册os.path属性
    MpModule_RegFunc(os_path_mod, "exists", os_exists);
    MpModule_RegFunc(os_path_mod, "dirname", os_path_dirname);

    // 注册os模块的属性
    MpModule_RegAttr(os_mod, "name", os_get_name());
    MpModule_RegAttr(os_mod, "path", os_path_mod);
    
    mp_reg_builtin_func("exists", os_exists);
}

