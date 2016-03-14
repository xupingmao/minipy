#include "include/tm.h"
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

#ifdef TM_NT
    #include <windows.h>
#else
    #include <unistd.h>
#endif

Object* getBuiltin(char* key) {
    if (!tm->init) {
        return NULL;
    }
    return dictGetByStr(tm->builtins, key);
}

void tmPutchar(int c){
    static char hex[] = {
        '0','1','2','3','4','5','6','7',
        '8','9','A','B','C','D','E','F'
    };

    if (isprint(c) || c == '\n' || c == '\t') {
        putchar(c);
    } else if(c == '\b') {
        putchar('\b');
    } else if(c=='\r'){
        /* output nothing */
    } else {
        int c0 = (c & 0xF0) >> 4;
        int c1 = c & 0x0F;
        printf("0x%c%c", hex[c0], hex[c1]);
    }
}

void tmPrint(Object o) {
    Object str = tmStr(o);
    int i;
    for(i = 0; i < GET_STR_LEN(str); i++) {
        tmPutchar(GET_STR(str)[i]);
    }
}

void tmPrintln(Object o) {
    tmPrint(o);
    puts("");
}



/**
 * based on C language standard.
 * d  -> int
 * f  -> double
 * c  -> char
 * s  -> char*
 * p  -> pointer
 * o  -> repl(object)
 * os -> object
 */
Object tmFormatVaList(char* fmt, va_list ap, int acquireNewLine) {
    int i;
    int len = strlen(fmt);
    Object str = stringNew("");
    int templ = 0;
    char* start = fmt;
    int istrans = 1;
    char buf[20];
    for (i = 0; i < len; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
            case 'd':
                sprintf(buf, "%d", va_arg(ap, int));
                str = stringAppendSz(str, buf);
                break;
            case 'f':
                /* ... will pass float as double */
                sprintf(buf, "%lf", va_arg(ap, double));
                str = stringAppendSz(str, buf);
                break;
                /* ... will pass char  as int */
            case 'c':
                str = stringAppendChar(str, va_arg(ap, int));
                break;
            case 's': {
                str = stringAppendSz(str, va_arg(ap, char*));
                break;
            }
            case 'P':
            case 'p': {
                sprintf(buf, "%p", va_arg(ap, void*));
                str = stringAppendSz(str, buf);
                break;
            }
            case 'o': {
                int next = fmt[i+1];
                Object v = va_arg(ap, Object);
                if (IS_STR(v) && next != 's') {
                    str = stringAppendChar(str, '"');
                }
                str = stringAppendObj(str, v);
                if (IS_STR(v) && next != 's') {
                    str = stringAppendChar(str, '"');
                }
                if (next == 's') {
                    i++;
                }
                break;
            }
            default:
                tmRaise("format, unknown pattern %c", fmt[i]);
                break;
            }
        } else {
            str = stringAppendChar(str, fmt[i]);
        }
    }
    if (acquireNewLine) {
        str = stringAppendChar(str, '\n');
    }
    return str;
}

Object tmFormat(char* fmt, ...) {
    va_list a;
    va_start(a, fmt);
    Object v = tmFormatVaList(fmt, a, 0);
    va_end(a);
    return v;
}

void tmPrintf(char* fmt, ...) {
    va_list a;
    va_start(a, fmt);
    tmPrint(tmFormatVaList(fmt, a, 0));
    va_end(a);
}


long get_rest_size(FILE* fp){
    long cur, end;
    cur = ftell(fp);
    fseek(fp, 0, SEEK_END);
    end = ftell(fp);
    fseek(fp, cur, SEEK_SET);
    return end - cur;
}


// vm-builtins

Object tmLoad(char* fname){
    FILE* fp = fopen(fname, "rb");
    if(fp == NULL){
        tmRaise("load: can not open file \"%s\"",fname);
        return NONE_OBJECT;
    }
    long len = get_rest_size(fp);
    if(len > MAX_FILE_SIZE){
        tmRaise("load: file too big to load, size = %d", (len));
        return NONE_OBJECT;
    }
    Object text = stringAlloc(NULL, len);
    char* s = GET_STR(text);
    fread(s, 1, len, fp);
    fclose(fp);
    return text;
}

Object tm_save(char*fname, Object content) {
    FILE* fp = fopen(fname, "wb");
    if (fp == NULL) {
        tmRaise("tm_save : can not save to file \"%s\"", fname);
    }
    char* txt = GET_STR(content);
    int len = GET_STR_LEN(content);
    fwrite(txt, 1, len, fp);
    fclose(fp);
    return NONE_OBJECT;
}


Object bf_input() {
    int i = 0;
    if (hasArg()) {
        tmPrint(argTakeObj("input"));
    }
    char buf[2048];
    memset(buf, '\0', sizeof(buf));
    fgets(buf, sizeof(buf), stdin);
    int len = strlen(buf);
    /* if last char is '\n', we shift it, mainly in tcc */
    if(buf[len-1]=='\n'){
        buf[len-1] = '\0';
    }
    return stringNew(buf);
}

Object bf_int() {
    Object v = argTakeObj("int");
    if (v.type == TYPE_NUM) {
        return tmNumber((int) GET_NUM(v));
    } else if (v.type == TYPE_STR) {
        return tmNumber((int) atof(GET_STR(v)));
    }
    tmRaise("int: %o can not be parsed to int ", v);
    return NONE_OBJECT;
}

Object bf_float() {
    Object v = argTakeObj("float");
    if (v.type == TYPE_NUM) {
        return v;
    } else if (v.type == TYPE_STR) {
        return tmNumber(atof(GET_STR(v)));
    }
    tmRaise("float: %o can not be parsed to float", v);
    return NONE_OBJECT;
}

/**
 *   load_module( name, code, mod_name = None )
 */
Object bf_load_module() {
    const char* szFnc = "load_module";
    Object file = argTakeStrObj(szFnc);
    Object code = argTakeStrObj(szFnc);
    Object mod;
    if (getArgsCount() == 3) {
        mod = moduleNew(file, argTakeStrObj(szFnc), code);
    } else {
        mod = moduleNew(file, file, code);
    }
    Object fnc = funcNew(mod, NONE_OBJECT, NULL);
    GET_FUNCTION(fnc)->code = (unsigned char*) GET_STR(code);
    GET_FUNCTION(fnc)->name = stringNew("#main");
    callFunction(fnc);
    return GET_MODULE(mod)->globals;
}


/* get globals */
Object bf_globals() {
    return GET_FUNCTION_GLOBALS(tm->frame->fnc);
}

/* get object type */

Object bf_exit() {
    longjmp(tm->frames->buf, 2);
    return NONE_OBJECT;
}

Object bf_gettype() {
    Object obj = argTakeObj("gettype");
    switch(TM_TYPE(obj)) {
        case TYPE_STR: return szToString("string");
        case TYPE_NUM: return szToString("number");
        case TYPE_LIST: return szToString("list");
        case TYPE_DICT: return szToString("dict");
        case TYPE_FUNCTION: return szToString("function");
        case TYPE_DATA: return szToString("data");
        case TYPE_NONE: return szToString("None");
        default: tmRaise("gettype(%o)", obj);
    }
    return NONE_OBJECT;
}

/**
 * bool istype(obj, strType);
 * this function is better than `gettype(obj) == 'string'`;
 * think that you want to check a `basetype` which contains both `number` and `string`;
 * so, a check function with less result is better.
 */
Object bf_istype() {
    Object obj = argTakeObj("istype");
    char* type = argTakeSz("istype");
    int isType = 0;
    switch(TM_TYPE(obj)) {
        case TYPE_STR: isType = strcmp(type, "string") == 0 ; break;
        case TYPE_NUM: isType = strcmp(type, "number") == 0 ; break;
        case TYPE_LIST: isType = strcmp(type, "list") == 0; break;
        case TYPE_DICT: isType = strcmp(type, "dict") == 0; break;
        case TYPE_FUNCTION: isType = strcmp(type, "function") == 0;break;
        case TYPE_DATA: isType = strcmp(type, "data") == 0; break;
        case TYPE_NONE: isType = strcmp(type, "None") == 0; break;
        default: tmRaise("gettype(%o)", obj);
    }
    return tmNumber(isType);
}

Object bf_chr() {
    int n = argTakeInt("chr");
    return stringChr(n);
}

Object bf_ord() {
    Object c = argTakeStrObj("ord");
    TM_ASSERT(GET_STR_LEN(c) == 1, "ord() expected a character");
    return tmNumber((unsigned char) GET_STR(c)[0]);
}

Object bf_code8() {
    int n = argTakeInt("code8");
    if (n < 0 || n > 255)
        tmRaise("code8(): expect number 0-255, but see %d", n);
    return stringChr(n);
}

Object bf_code16() {
    int n = argTakeInt("code16");
    if (n < 0 || n > 0xffff)
        tmRaise("code16(): expect number 0-0xffff, but see %x", n);
    Object nchar = stringAlloc(NULL, 2);
    code16((unsigned char*) GET_STR(nchar), n);
    return nchar;
}

Object bf_code32() {
    int n = argTakeInt("code32");
    Object c = stringAlloc(NULL, 4);
    code32((unsigned char*) GET_STR(c), n);
    return c;
}

Object bf_raise() {
    if (getArgsCount() == 0) {
        tmRaise("raise");
    } else {
        tmRaise("%s", argTakeSz("raise"));
    }
    return NONE_OBJECT;
}

Object bf_system() {
    Object m = argTakeStrObj("system");
    int rs = system(GET_STR(m));
    return tmNumber(rs);
}

Object bf_str() {
    Object a = argTakeObj("str");
    return tmStr(a);
}

Object bf_len() {
    Object o = argTakeObj("len");
    int len = -1;
    switch (TM_TYPE(o)) {
    case TYPE_STR:
        len = GET_STR_LEN(o);
    case TYPE_LIST:
        len = LIST_LEN(o);
    case TYPE_DICT:
        len = DICT_LEN(o);
    }
    if (len < 0) {
        tmRaise("tmLen: %o has no attribute len", o);
    }
    return tmNumber(len);
}

Object bf_print() {
    int i = 0;
    while (hasArg()) {
        tmPrint(argTakeObj("print"));
        if (hasArg()) {
            putchar(' ');
        }
    }
    putchar('\n');
    return NONE_OBJECT;
}

Object bf_load(Object p){
    Object fname = argTakeStrObj("load");
    return tmLoad(GET_STR(fname));
}
Object bf_save(){
    Object fname = argTakeStrObj("<save name>");
    return tm_save(GET_STR(fname), argTakeStrObj("<save content>"));
}

Object bfRemove(){
    Object fname = argTakeStrObj("remove");
    int flag = remove(GET_STR(fname));
    if(flag) {
        return tmNumber(0);
    } else {
        return tmNumber(1);
    }
}

Object bf_apply() {
    Object func = argTakeObj("apply");
    if (NOT_FUNC(func) && NOT_DICT(func)) {
        tmRaise("apply: expect function or dict");
    }
    Object args = argTakeObj("apply");
    argStart();
    if (IS_NONE(args)) {
    } else if(IS_LIST(args)) {
        int i;for(i = 0; i < LIST_LEN(args); i++) {
            argPush(LIST_NODES(args)[i]);
        }
    } else {
        tmRaise("apply: expect list arguments or None, but see %o", args);
        return NONE_OBJECT;
    }
    return callFunction(func);
}

Object bf_write() {
    Object fmt = argTakeObj("write");
    Object str = tmStr(fmt);
    char* s = GET_STR(str);
    int len = GET_STR_LEN(str);
    int i;
    // for(i = 0; i < len; i++) {
        // tmPutchar(s[i]);
        // putchar is very very slow
        // when print 80 * 30 chars  
        //     ==>  putcharTime=126, printfTime=2, putcTime=129
        // putchar(s[i]);
        // buffer[i] = s[i];
    // }
    printf("%s", s);
    // return arrayToList(2, tmNumber(t2-t1), tmNumber(t3-t2));
    return NONE_OBJECT;
}

Object bf_pow() {
    double base = argTakeDouble("pow");
    double y = argTakeDouble("pow");
    return tmNumber(pow(base, y));
}

typedef struct RangeIter {
    DATA_HEAD
    long cur;
    long inc;
    long stop;
    Object cur_obj;
}RangeIter;

Object* rangeNext(RangeIter* data) {
    long cur = data->cur;
    if (data->inc > 0 && cur < data->stop) {
        data->cur += data->inc;
        data->cur_obj = tmNumber(cur);
        return &data->cur_obj;
    } else if (data->inc < 0 && cur > data->stop) {
        data->cur += data->inc;
        data->cur_obj = tmNumber(cur);
        return &data->cur_obj;
    }
    return NULL;
}

static DataProto rangeIter;
DataProto* getRangeIterProto() {
    if(!rangeIter.init) {
        initDataProto(&rangeIter);
        rangeIter.next = rangeNext;
        rangeIter.dataSize = sizeof(RangeIter);
    }
    return &rangeIter;
}

Object bf_range() {
    long start = 0;
    long end = 0;
    int inc;
    static const char* szFunc = "range";
    switch (tm->arg_cnt) {
    case 1:
        start = 0;
        end = (long)argTakeDouble(szFunc);
        inc = 1;
        break;
    case 2:
        start = (long)argTakeDouble(szFunc);
        end = (long)argTakeDouble(szFunc);
        inc = 1;
        break;
    case 3:
        start = (long)argTakeDouble(szFunc);
        end = (long)argTakeDouble(szFunc);
        inc = (long)argTakeDouble(szFunc);
        break;
    default:
        tmRaise("range([n, [ n, [n]]]), but see %d arguments",
                tm->arg_cnt);
    }
    if (inc == 0)
        tmRaise("range(): increment can not be 0!");
    if (inc * (end - start) < 0)
        tmRaise("range(%d, %d, %d): not valid range!", start, end, inc);
    Object data = dataNew(sizeof(RangeIter));
    RangeIter *iterator = (RangeIter*) GET_DATA(data);
    iterator->cur = start;
    iterator->stop = end;
    iterator->inc = inc;
    iterator->proto = getRangeIterProto();
    return data;
}

Object bf_mmatch() {
    char* str = argTakeSz("mmatch");
    int start = argTakeInt("mmatch");
    Object o_dst = argTakeStrObj("mmatch");
    char* dst = GET_STR(o_dst);
    int size = GET_STR_LEN(o_dst);
    return tmNumber(strncmp(str+start, dst, size) == 0);
}


/***********************************
* built-in functions for developers
***********************************/

Object bfInspectPtr() {
    double _ptr = argTakeDouble("inspectPtr");
    int idx = argTakeInt("inspectPtr");
    char* ptr = (char*)(long long)_ptr;
    return stringChr(ptr[idx]);
}

Object bfGetCurrentFrame() {
    Object frameInfo = dictNew();
    dictSetByStr(frameInfo, "function", tm->frame->fnc);
    // dictSetByStr(frameInfo, "pc", tmNumber((long long)tm->frame->pc));
    dictSetByStr(frameInfo, "index", tmNumber((long long) (tm->frame - tm->frames)));
    return frameInfo;
}

Object bfVmopt() {
    char* opt = argTakeSz("vminfo");
    if (strcmp(opt, "gc") == 0) {
        gcFull();
    } else if (strcmp(opt, "help") == 0) {
        return szToString("gc, help");
    } else if (strcmp(opt, "frame.local") == 0) {
        int fidx = argTakeInt("vminfo");
        int lidx = argTakeInt("vminfo");
        return objGetlocal(fidx, lidx);
    } else if (strcmp(opt, "frame.stack") == 0) {
        int fidx = argTakeInt("vminfo");
        int sidx = argTakeInt("vminfo");
        return objGetstack(fidx, sidx);
    } else if (strcmp(opt, "frame.index") == 0) {
        return tmNumber(tm->frame-tm->frames);
    } else if (strcmp(opt, "frame.info") == 0) {
        int fidx = argTakeInt("vminfo");
        TmFrame *f = objGetframe(fidx);
        Object info = dictNew();
        tmSetattr(info, "maxlocals", tmNumber(f->maxlocals));
        tmSetattr(info, "stacksize", tmNumber(f->top - f->stack));
        tmSetattr(info, "func", f->fnc);
        tmSetattr(info, "fname", tmGetfname(f->fnc));
        tmSetattr(info, "lineno", tmNumber(f->lineno));
        return info;
    } else {
        tmRaise("invalid opt %s", opt);
    }
    return NONE_OBJECT;
}

Object bfGetVmInfo() {
    Object tmInfo = dictNew();
    dictSetByStr(tmInfo, "name", stringNew("tm"));
    dictSetByStr(tmInfo, "vm_size", tmNumber(sizeof(TmVM)));
    dictSetByStr(tmInfo, "obj_size", tmNumber(sizeof(Object)));
    dictSetByStr(tmInfo, "int_size", tmNumber(sizeof(int)));
    dictSetByStr(tmInfo, "long_size", tmNumber(sizeof(long)));
    dictSetByStr(tmInfo, "long_long_size", tmNumber(sizeof(long long)));
    dictSetByStr(tmInfo, "float_size", tmNumber(sizeof(float)));
    dictSetByStr(tmInfo, "double_size", tmNumber(sizeof(double)));
    dictSetByStr(tmInfo, "jmp_buf_size", tmNumber(sizeof(jmp_buf)));
    dictSetByStr(tmInfo, "total_obj_len", tmNumber(tm->all->len));
    dictSetByStr(tmInfo, "alloc_mem", tmNumber(tm->allocated));
    dictSetByStr(tmInfo, "gc_threshold", tmNumber(tm->gcThreshold));
    dictSetByStr(tmInfo, "frame_index", tmNumber(tm->frame - tm->frames));
    dictSetByStr(tmInfo, "consts_len", tmNumber(DICT_LEN(tm->constants)));
    return tmInfo;
}

long tmClock() {
#ifdef TM_NT
    return clock();
#else
    return (double)clock()/1000;
#endif
}


Object bf_clock() {
    return tmNumber(tmClock());
}

Object bf_time0() {
    return tmNumber(time(0));
}

Object bfSleep() {
    int i = 0;
    int t = argTakeInt("sleep");
#ifdef _WINDOWS_H
    Sleep(t);
#else
    sleep(t);
#endif
    return NONE_OBJECT;
}

Object bfAddObjMethod() {
    static const char* szFunc = "add_obj_method";
    Object type = argTakeStrObj(szFunc);
    Object fname = argTakeStrObj(szFunc);
    Object fnc = argTakeFuncObj(szFunc);
    char*s = GET_STR(type);
    if (strcmp(s, "str") == 0) {
        objSet(tm->str_proto, fname, fnc);
    } else if (strcmp(s, "list") == 0) {
        objSet(tm->list_proto, fname, fnc);
    } else if (strcmp(s, "dict") == 0) {
        objSet(tm->dict_proto, fname, fnc);
    } else {
        tmRaise("add_obj_method: not valid object type, expect str, list, dict");
    }
    return NONE_OBJECT;
}

Object bfReadFile() {
    static const char* szFunc = "readFile";
    char c;
    char* fname = argTakeSz(szFunc);
    int nsize = argTakeInt(szFunc);
    char buf[1024];
    int i;
    int end = 0;
    Object func;
    if (nsize < 0 || nsize > 1024) {
        tmRaise("%s: can not set bufsize beyond [1, 1024]",  szFunc);
    }
    func = argTakeFuncObj(szFunc);
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
        argPush(stringAlloc(buf, i));
        callFunction(func);
        if (end) {
            break;
        }
    }
    fclose(fp);
    return NONE_OBJECT;
}

Object bfIter() {
    Object func = argTakeObj("iter");
    return iterNew(func);
}

Object bfNext() {
    Object iter = argTakeDataObj("next");
    Object *ret = nextPtr(iter);
    if (ret == NULL) {
        tmRaise("<<next end>>");
        return NONE_OBJECT;
    } else {
        return *ret;
    }
}

Object bfSetVMState() {
    int state = argTakeInt("setVMState");
    switch(state) {
        case 0:tm->debug = 0;break;
        case 1:tm->debug = 1;break;
    }
    return NONE_OBJECT;
}

Object bfGetConstIdx() {
    Object key = argTakeObj("getConstIdx");
    SET_IDX(key, 0);
    int i = dictSet(tm->constants, key, NONE_OBJECT);
    SET_IDX(GET_CONST(i), i);
    return tmNumber(i);
}

Object bfGetConst() {
    int num = argTakeInt("getConst");
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
    return tmNumber(DICT_LEN(tm->constants));
}

Object bfGetExList() {
    return tm->exList;
}

Object bfExists(){
    Object _fname = argTakeStrObj("exists");
    char* fname = GET_STR(_fname);
    FILE*fp = fopen(fname, "rb");
    if(fp == NULL) return NUMBER_FALSE;
    fclose(fp);
    return NUMBER_TRUE;
}

Object bfStat(){
    const char *s = argTakeSz("stat");
    struct stat stbuf;
    if (!stat(s,&stbuf)) { 
        Object st = dictNew();
        dictSetByStr(st, "st_mtime", tmNumber(stbuf.st_mtime));
        dictSetByStr(st, "st_atime", tmNumber(stbuf.st_atime));
        dictSetByStr(st, "st_ctime", tmNumber(stbuf.st_ctime));
        dictSetByStr(st, "st_size" , tmNumber(stbuf.st_size));
        dictSetByStr(st, "st_mode",  tmNumber(stbuf.st_mode));
        dictSetByStr(st, "st_nlink", tmNumber(stbuf.st_nlink));
        dictSetByStr(st, "st_dev",   tmNumber(stbuf.st_dev));
        dictSetByStr(st, "st_ino",   tmNumber(stbuf.st_ino));
        dictSetByStr(st, "st_uid",   tmNumber(stbuf.st_uid));
        dictSetByStr(st, "st_gid",   tmNumber(stbuf.st_gid));
        return st;
    }
    tmRaise("stat(%s), file not exists or accessable.",s);
    return NONE_OBJECT;
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
    return stringNew(buf);
}

Object bfChdir() {
    const char* szFunc = "chdir";
    char *path = argTakeSz(szFunc);
    int r = chdir(path);
    if (r != 0) {
        tmRaise("%s: -- fatal error, can not chdir(\"%s\")", szFunc, path);
    } 
    return NONE_OBJECT;
}

Object bfGetOsName() {
    const char* szFunc = "getosname";
#ifdef _WINDOWS_H
    return szToString("nt");
#else
    return szToString("posix");
#endif
}

Object bfListdir() {
    Object list = listNew(10);
    Object path = argTakeStrObj("listdir");
#ifdef _WINDOWS_H
    WIN32_FIND_DATA FindFileData;
    Object _path = objAdd(path, stringNew("\\*.*"));
    HANDLE hFind = FindFirstFile(GET_STR(_path), &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        tmRaise("%s is not a directory", path);
    }
    do {
        if (strcmp(FindFileData.cFileName, "..")==0 || strcmp(FindFileData.cFileName, ".") == 0) {
            continue;
        }
        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // do nothing.
        }
        Object file = stringNew(FindFileData.cFileName);
        objAppend(list, file);
    } while (FindNextFile(hFind, &FindFileData));
    FindClose(hFind);
#else
    tmRaise("listdir not implemented in posix.");
#endif
    return list;
}

Object bfTraceback() {
    traceback();
    return NONE_OBJECT;
}

/**
 * create a object in tm
 */ 
Object bf_newobj() {
    Object obj = dictNew();
    return obj;
}

/**
 * random
 */
Object bf_random() {
    static long seed = 0;
    if (seed == 0) {
        seed = time(NULL);
        srand(seed);
    }
    int n = rand() % 77;
    // printf("%d\n", n);
    double val = (double)((double) n / (double)77);
    return tmNumber(val);
}

void builtinsInit() {
    regBuiltinFunc("load", bf_load);
    regBuiltinFunc("save", bf_save);
    regBuiltinFunc("remove", bfRemove);
    regBuiltinFunc("print", bf_print);
    regBuiltinFunc("write", bf_write);
    regBuiltinFunc("input", bf_input);
    regBuiltinFunc("str", bf_str);
    regBuiltinFunc("int", bf_int);
    regBuiltinFunc("float", bf_float);
    regBuiltinFunc("load_module", bf_load_module);
    regBuiltinFunc("globals", bf_globals);
    regBuiltinFunc("len", bf_len);
    regBuiltinFunc("exit", bf_exit);
    regBuiltinFunc("gettype", bf_gettype);
    regBuiltinFunc("istype", bf_istype);
    regBuiltinFunc("chr", bf_chr);
    regBuiltinFunc("ord", bf_ord);
    regBuiltinFunc("code8", bf_code8);
    regBuiltinFunc("code16", bf_code16);
    regBuiltinFunc("code32", bf_code32);
    regBuiltinFunc("raise", bf_raise);
    regBuiltinFunc("system", bf_system);
    regBuiltinFunc("apply", bf_apply);
    regBuiltinFunc("pow", bf_pow);
    regBuiltinFunc("range", bf_range);
    regBuiltinFunc("mmatch", bf_mmatch);
    regBuiltinFunc("newobj", bf_newobj);
    regBuiltinFunc("random", bf_random);
    
    /* functions which has impact on vm follow camel case */
    regBuiltinFunc("getConstIdx", bfGetConstIdx);
    regBuiltinFunc("getConst", bfGetConst);
    regBuiltinFunc("getConstLen", bfGetConstLen);
    regBuiltinFunc("getExList", bfGetExList);
    regBuiltinFunc("setVMState", bfSetVMState);
    regBuiltinFunc("inspectPtr", bfInspectPtr);
    regBuiltinFunc("getCurrentFrame", bfGetCurrentFrame);
    regBuiltinFunc("vmopt", bfVmopt);
    regBuiltinFunc("getVmInfo", bfGetVmInfo);
    regBuiltinFunc("traceback", bfTraceback);

    regBuiltinFunc("clock", bf_clock);
    regBuiltinFunc("time0", bf_time0);
    regBuiltinFunc("add_obj_method", bfAddObjMethod);
    regBuiltinFunc("readFile", bfReadFile);
    regBuiltinFunc("iter", bfIter);
    regBuiltinFunc("next", bfNext);
    regBuiltinFunc("sleep", bfSleep);
    
    regBuiltinFunc("exists", bfExists);
    regBuiltinFunc("stat", bfStat);
    regBuiltinFunc("getcwd", bfGetcwd);
    regBuiltinFunc("chdir", bfChdir);
    regBuiltinFunc("getosname", bfGetOsName);
    regBuiltinFunc("listdir", bfListdir);
}

