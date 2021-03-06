#include "../include/mp.h"

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
    char *path = arg_take_cstr(sz_func);
    int r = chdir(path);
    if (r != 0) {
        mp_raise("%s: -- fatal error, can not chdir(\"%s\")", sz_func, path);
    } 
    return NONE_OBJECT;
}


MpObj os_listdir() {
    MpObj list = list_new(10);
    MpObj path = arg_take_str_obj("listdir");
#ifdef _WIN32
    WIN32_FIND_DATA Find_file_data;
    MpObj _path = obj_add(path, string_new("\\*.*"));
    HANDLE h_find = FindFirstFile(GET_CSTR(_path), &Find_file_data);
    if (h_find == INVALID_HANDLE_VALUE) {
        mp_raise("%s is not a directory", path);
    }
    do {
        if (strcmp(Find_file_data.cFileName, "..")==0 || strcmp(Find_file_data.cFileName, ".") == 0) {
            continue;
        }
        if (Find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // do nothing.
        }
        MpObj file = string_new(Find_file_data.cFileName);
        obj_append(list, file);
    } while (FindNextFile(h_find, &Find_file_data));
    FindClose(h_find);
#else
    mp_raise("listdir not implemented in posix.");
#endif
    return list;
}

MpObj os_stat(){
    const char *s = arg_take_cstr("stat");
    struct stat stbuf;
    if (!stat(s,&stbuf)) { 
        MpObj st = dict_new();
        obj_set_by_cstr(st, "st_mtime", number_obj(stbuf.st_mtime));
        obj_set_by_cstr(st, "st_atime", number_obj(stbuf.st_atime));
        obj_set_by_cstr(st, "st_ctime", number_obj(stbuf.st_ctime));
        obj_set_by_cstr(st, "st_size" , number_obj(stbuf.st_size));
        obj_set_by_cstr(st, "st_mode",  number_obj(stbuf.st_mode));
        obj_set_by_cstr(st, "st_nlink", number_obj(stbuf.st_nlink));
        obj_set_by_cstr(st, "st_dev",   number_obj(stbuf.st_dev));
        obj_set_by_cstr(st, "st_ino",   number_obj(stbuf.st_ino));
        obj_set_by_cstr(st, "st_uid",   number_obj(stbuf.st_uid));
        obj_set_by_cstr(st, "st_gid",   number_obj(stbuf.st_gid));
        return st;
    }
    mp_raise("stat(%s), file not exists or accessable.",s);
    return NONE_OBJECT;
}

MpObj os_exists(){
    MpObj _fname = arg_take_str_obj("exists");
    char* fname = GET_CSTR(_fname);
    FILE*fp = fopen(fname, "rb");
    if(fp == NULL) return tm->_FALSE;
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

void os_mod_init() {
    MpObj os_mod = dict_new();
    obj_set_by_cstr(tm->modules, "os", os_mod);
    reg_mod_func(os_mod, "getcwd",  os_getcwd);
    reg_mod_func(os_mod, "listdir", os_listdir);
    reg_mod_func(os_mod, "chdir",   os_chdir);
    reg_mod_func(os_mod, "stat",    os_stat);
    reg_mod_func(os_mod, "exists",  os_exists);
    
    reg_builtin_func("exists", os_exists);
}

