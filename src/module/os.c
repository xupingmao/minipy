#include "../include/mp.h"

Object os_getcwd() {
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
        tm_raise("%s: error -- %s", sz_func, msg);
    }
    return string_new(buf);
}


Object os_chdir() {
    const char* sz_func = "chdir";
    char *path = arg_take_sz(sz_func);
    int r = chdir(path);
    if (r != 0) {
        tm_raise("%s: -- fatal error, can not chdir(\"%s\")", sz_func, path);
    } 
    return NONE_OBJECT;
}


Object os_listdir() {
    Object list = list_new(10);
    Object path = arg_take_str_obj("listdir");
#ifdef _WIN32
    WIN32_FIND_DATA Find_file_data;
    Object _path = obj_add(path, string_new("\\*.*"));
    HANDLE h_find = FindFirstFile(GET_STR(_path), &Find_file_data);
    if (h_find == INVALID_HANDLE_VALUE) {
        tm_raise("%s is not a directory", path);
    }
    do {
        if (strcmp(Find_file_data.cFileName, "..")==0 || strcmp(Find_file_data.cFileName, ".") == 0) {
            continue;
        }
        if (Find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // do nothing.
        }
        Object file = string_new(Find_file_data.cFileName);
        obj_append(list, file);
    } while (FindNextFile(h_find, &Find_file_data));
    FindClose(h_find);
#else
    tm_raise("listdir not implemented in posix.");
#endif
    return list;
}

Object os_stat(){
    const char *s = arg_take_sz("stat");
    struct stat stbuf;
    if (!stat(s,&stbuf)) { 
        Object st = dict_new();
        dict_set_by_str(st, "st_mtime", tm_number(stbuf.st_mtime));
        dict_set_by_str(st, "st_atime", tm_number(stbuf.st_atime));
        dict_set_by_str(st, "st_ctime", tm_number(stbuf.st_ctime));
        dict_set_by_str(st, "st_size" , tm_number(stbuf.st_size));
        dict_set_by_str(st, "st_mode",  tm_number(stbuf.st_mode));
        dict_set_by_str(st, "st_nlink", tm_number(stbuf.st_nlink));
        dict_set_by_str(st, "st_dev",   tm_number(stbuf.st_dev));
        dict_set_by_str(st, "st_ino",   tm_number(stbuf.st_ino));
        dict_set_by_str(st, "st_uid",   tm_number(stbuf.st_uid));
        dict_set_by_str(st, "st_gid",   tm_number(stbuf.st_gid));
        return st;
    }
    tm_raise("stat(%s), file not exists or accessable.",s);
    return NONE_OBJECT;
}

Object os_exists(){
    Object _fname = arg_take_str_obj("exists");
    char* fname = GET_STR(_fname);
    FILE*fp = fopen(fname, "rb");
    if(fp == NULL) return tm->_FALSE;
    fclose(fp);
    return tm->_TRUE;
}

void os_mod_init() {
    Object os_mod = dict_new();
    dict_set_by_str(tm->modules, "os", os_mod);
    reg_mod_func(os_mod, "getcwd",  os_getcwd);
    reg_mod_func(os_mod, "listdir", os_listdir);
    reg_mod_func(os_mod, "chdir",   os_chdir);
    reg_mod_func(os_mod, "stat",    os_stat);
    reg_mod_func(os_mod, "exists",  os_exists);
    
    reg_builtin_func("exists", os_exists);
}

