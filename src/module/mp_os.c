#include "../include/mp.h"
#include <dirent.h>

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

#ifndef _WIN32
static void os_listdir_unix(MpObj result, char *path) {
    DIR *db = NULL;
    char filename[128];
    struct dirent *p;

    // 打开文件夹
    db = opendir(path);
    if(db==NULL) {
        mp_raise("os.listdir: opendir failed");
        return;
    }

    memset(filename,0,128);

    while ((p=readdir(db))) {
        if((strcmp(p->d_name,".")==0)||(strcmp(p->d_name,"..")==0)) {
            continue;
        } else {
            MpObj name_obj = string_new(p->d_name);
            mp_append(result, name_obj);
        }
        memset(filename,0,64);
    }
    closedir(db);
}
#endif


MpObj os_listdir() {
    MpObj list = list_new(10);
    MpObj path = mp_take_str_obj_arg("listdir");
#ifdef _WIN32
    WIN32_FIND_DATA Find_file_data;
    MpObj _path = obj_add(path, string_new("\\*.*"));
    HANDLE h_find = FindFirstFile(GET_CSTR(_path), &Find_file_data);
    if (h_find == INVALID_HANDLE_VALUE) {
        mp_raise("os.listdir: %s is not a directory", path);
    }
    do {
        if (strcmp(Find_file_data.cFileName, "..")==0 || strcmp(Find_file_data.cFileName, ".") == 0) {
            continue;
        }
        if (Find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // do nothing.
        }
        MpObj file = string_new(Find_file_data.cFileName);
        mp_append(list, file);
    } while (FindNextFile(h_find, &Find_file_data));
    FindClose(h_find);
#else
    os_listdir_unix(list, GET_CSTR(path));
#endif
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
    FILE*fp = fopen(fname, "rb");
    if(fp == NULL) {
        return tm->_FALSE;
    }
    fclose(fp);
    return tm->_TRUE;
}

MpObj os_path_dirname0(MpObj fpath) {
    mp_assert_type(fpath, TYPE_STR, "os_path_dirname");

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

MpObj os_path_join0(MpObj dirname, MpObj fname) {
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

/**
 * @brief OS模块
 */
void mp_os_init() {
    MpObj os_mod = mp_new_native_module("os");

    MpModule_RegFunc(os_mod, "getcwd",  os_getcwd);
    MpModule_RegFunc(os_mod, "listdir", os_listdir);
    MpModule_RegFunc(os_mod, "chdir",   os_chdir);
    MpModule_RegFunc(os_mod, "stat",    os_stat);
    MpModule_RegFunc(os_mod, "exists",  os_exists);
    MpModule_RegFunc(os_mod, "system",  os_system);

    // 注册os模块的属性
    MpModule_RegAttr(os_mod, "name", os_get_name());
    
    mp_reg_builtin_func("exists", os_exists);
}

