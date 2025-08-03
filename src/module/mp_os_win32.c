
#include <Windows.h>
#include "../include/mp.h"

static void os_listdir_impl(MpObj list, MpObj path) {
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
}

int MpOS_mkdir(char* fname) {
    return mkdir(fname);
}
