#include "include/builtins.h"
#include "include/tmstring.h"
#include "include/gc.h"
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

/***********************************
* built-in functions for developers
***********************************/

Object bfInspectPtr() {
	double _ptr = getNumArg("inspectPtr");
	int idx = getIntArg("inspectPtr");
    char* ptr = (char*)(long long)_ptr;
	return string_chr(ptr[idx]);
}

Object bfGetCurrentFrame() {
	Object frameInfo = dict_new();
	dictSetByStr(frameInfo, "function", tm->frame->fnc);
	// dictSetByStr(frameInfo, "pc", tm_number((long long)tm->frame->pc));
    dictSetByStr(frameInfo, "index", tm_number((long long) (tm->frame - tm->frames)));
	return frameInfo;
}

Object bfVmOpt() {
    char* opt = getSzArg("vminfo");
    if (strcmp(opt, "gc") == 0) {
        gcFull();
    } else if (strcmp(opt, "help") == 0) {
        return string_static("gc, help");
    }
	return NONE_OBJECT;
}

Object bfGetVmInfo() {
    Object tmInfo = dict_new();
    dictSetByStr(tmInfo, "name", string_new("tm"));
    dictSetByStr(tmInfo, "vm_size", tm_number(sizeof(TmVM)));
    dictSetByStr(tmInfo, "obj_size", tm_number(sizeof(Object)));
    dictSetByStr(tmInfo, "int_size", tm_number(sizeof(int)));
    dictSetByStr(tmInfo, "long_size", tm_number(sizeof(long)));
    dictSetByStr(tmInfo, "long_long_size", tm_number(sizeof(long long)));
    dictSetByStr(tmInfo, "float_size", tm_number(sizeof(float)));
    dictSetByStr(tmInfo, "double_size", tm_number(sizeof(double)));
    dictSetByStr(tmInfo, "jmp_buf_size", tm_number(sizeof(jmp_buf)));
    dictSetByStr(tmInfo, "total_obj_len", tm_number(tm->all->len));
    dictSetByStr(tmInfo, "alloc_mem", tm_number(tm->allocated));
    dictSetByStr(tmInfo, "gc_threshold", tm_number(tm->gcThreshold));
    dictSetByStr(tmInfo, "frame_index", tm_number(tm->frame - tm->frames));
    dictSetByStr(tmInfo, "consts_len", tm_number(DICT_LEN(tm->constants)));
    return tmInfo;
}

Object bfClock() {
#ifdef TM_NT
	return tm_number(clock());
#else
	return tm_number((double)clock()/1000);
#endif
}

Object bfSleep() {
    int i = 0;
    int t = getIntArg("sleep");
#ifdef _WINDOWS_H
    Sleep(t);
#else
    sleep(t);
#endif
    return NONE_OBJECT;
}

Object bfAddObjMethod() {
    static const char* szFunc = "add_obj_method";
	Object type = getStrArg(szFunc);
	Object fname = getStrArg(szFunc);
	Object fnc = getFuncArg(szFunc);
	char*s = GET_STR(type);
	if (strcmp(s, "str") == 0) {
		tmSet(CLASS_STRING, fname, fnc);
	} else if (strcmp(s, "list") == 0) {
		tmSet(CLASS_LIST, fname, fnc);
	} else if (strcmp(s, "dict") == 0) {
		tmSet(CLASS_DICT, fname, fnc);
	} else {
		tmRaise("add_obj_method: not valid object type, expect str, list, dict");
	}
	return NONE_OBJECT;
}

Object bfReadFile() {
    static const char* szFunc = "readFile";
    char c;
    char* fname = getSzArg(szFunc);
    int nsize = getIntArg(szFunc);
    char buf[1024];
    int i;
    int end = 0;
    Object func;
    if (nsize < 0 || nsize > 1024) {
        tmRaise("%s: can not set bufsize beyond [1, 1024]",  szFunc);
    }
    func = getFuncArg(szFunc);
    FILE* fp = fopen(fname, "rb");
    if (fp == NULL) {
        tmRaise("%s: can not open file %s", szFunc, fname);
    }
    while (1) {
        argStart();
        for (i = 0; i < nsize; i++) {
            if ((c = fgetc(fp)) != EOF) {
                buf[i] = c;
            } else {
                end = 1;
                break;
            }
        }
        pushArg(string_alloc(buf, i));
        callFunction(func);
        if (end) {
            break;
        }
    }
    fclose(fp);
    return NONE_OBJECT;
}

Object bfIter() {
    Object func = getObjArg("iter");
    return iter_new(func);
}

Object bfNext() {
    Object iter = getDataArg("next");
    Object *ret = tm_next(iter);
    if (ret == NULL) {
        tmRaise("");
        return NONE_OBJECT;
    } else {
        return *ret;
    }
}

Object bfSetVMState() {
    int state = getIntArg("setVMState");
    switch(state) {
        case 0:tm->debug = 0;break;
        case 1:tm->debug = 1;break;
    }
    return NONE_OBJECT;
}

Object bfGetConstIdx() {
    Object key = getObjArg("getConstIdx");
    key.idx = 0;
    int i = dictSet(tm->constants, key, NONE_OBJECT);
    GET_DICT(tm->constants)->nodes[i].key.idx = i;
    /*
    int i = listIndex(tm->constants, value);
    if (i < 0) {
        APPEND(tm->constants, value);
        i = listIndex(tm->constants, value);
    }*/
    /*DEBUG tmPrintf("LoadConst %d:%o\n", i, LIST_GET(tm->constants,i)); */
    /* here can check again in case of memory leak */
    return tm_number(i);
}

Object bfGetConst() {
    int num = getIntArg("getConst");
    int idx = num;
    if (num < 0) {
        idx += DICT_LEN(tm->constants);
    }
    if (idx < 0 || idx >= DICT_LEN(tm->constants)) {
        tmRaise("getConst(idx): out of range [%d]", num);
    }
    return GET_CONST(idx);
}
/* for save */
Object bfGetConstLen() {
    return tm_number(DICT_LEN(tm->constants));
}

Object bfGetExList() {
    return tm->exList;
}

Object bfExists(){
    Object _fname = getStrArg("exists");
    char* fname = GET_STR(_fname);
    FILE*fp = fopen(fname, "rb");
    if(fp == NULL) return NUMBER_FALSE;
    fclose(fp);
    return NUMBER_TRUE;
}

Object bfStat(){
    const char *s = getSzArg("stat");
    struct stat stbuf;
    if (!stat(s,&stbuf)) { 
        Object st = dict_new();
        dictSetByStr(st, "st_mtime", tm_number(stbuf.st_mtime));
        dictSetByStr(st, "st_atime", tm_number(stbuf.st_atime));
        dictSetByStr(st, "st_ctime", tm_number(stbuf.st_ctime));
        dictSetByStr(st, "st_size" , tm_number(stbuf.st_size));
        dictSetByStr(st, "st_mode",  tm_number(stbuf.st_mode));
        dictSetByStr(st, "st_nlink", tm_number(stbuf.st_nlink));
        dictSetByStr(st, "st_dev",   tm_number(stbuf.st_dev));
        dictSetByStr(st, "st_ino",   tm_number(stbuf.st_ino));
        dictSetByStr(st, "st_uid",   tm_number(stbuf.st_uid));
        dictSetByStr(st, "st_gid",   tm_number(stbuf.st_gid));
        return st;
    }
    tmRaise("stat(%s), file not exists or accessable.",s);
    return NONE_OBJECT;
}

Object bfGetFuncCode() {
    Object func = getFuncArg("get_func_code");
    if (IS_NATIVE(func)) {
        return NONE_OBJECT;
    }
    unsigned char* code = GET_FUNCTION(func)->code;
    int len = 0;
    while (code[len] != TM_EOF) {
        len += 3;
    }
    len += 3; /* TM_EOF */
    return string_alloc((char*)code, len);
}

Object bfGetcwd() {
    const char* szFunc = "getcwd";
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
        tmRaise("%s: error -- %s", szFunc, msg);
    }
    return string_new(buf);
}

Object bfChdir() {
    const char* szFunc = "chdir";
    char *path = getSzArg(szFunc);
    int r = chdir(path);
    if (r != 0) {
        tmRaise("%s: -- fatal error, can not chdir(\"%s\")", szFunc, path);
    } 
    return NONE_OBJECT;
}

Object bfGetOsName() {
    const char* szFunc = "getosname";
#ifdef _WINDOWS_H
    return string_static("nt");
#else
    return string_static("posix");
#endif
}

void regBuiltinsFunc2() {
    /* functions which has impact on vm follow camel case */
    regBuiltinFunc("getConstIdx", bfGetConstIdx);
    regBuiltinFunc("getConst", bfGetConst);
    regBuiltinFunc("getConstLen", bfGetConstLen);
    regBuiltinFunc("getExList", bfGetExList);
    regBuiltinFunc("setVMState", bfSetVMState);
    regBuiltinFunc("inspectPtr", bfInspectPtr);
	regBuiltinFunc("getCurrentFrame", bfGetCurrentFrame);
	regBuiltinFunc("vmopt", bfVmOpt);
    regBuiltinFunc("getVmInfo", bfGetVmInfo);

    regBuiltinFunc("clock", bfClock);
    regBuiltinFunc("add_obj_method", bfAddObjMethod);
    regBuiltinFunc("get_func_code", bfGetFuncCode);
    regBuiltinFunc("readFile", bfReadFile);
    regBuiltinFunc("iter", bfIter);
    regBuiltinFunc("next", bfNext);
    regBuiltinFunc("sleep", bfSleep);
    
    regBuiltinFunc("exists", bfExists);
	regBuiltinFunc("stat", bfStat);
    regBuiltinFunc("getcwd", bfGetcwd);
    regBuiltinFunc("chdir", bfChdir);
    regBuiltinFunc("getosname", bfGetOsName);
}

