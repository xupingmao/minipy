#ifdef _POSIX_VERSION
#include "../include/mp.h"
#include <dirent.h>
#include <sys/stat.h>

static void os_listdir_impl(MpObj result, MpObj path_obj) {
    char* path = GET_CSTR(path_obj);
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

int MpOS_mkdir(char* fname) {
    mode_t mode = 0755;
    return mkdir(fname, mode);
}

#endif
