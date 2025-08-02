#include "../include/mp.h"

static MpObj bf_file_close();
static MpObj bf_file_read();
static MpObj bf_file_write();
static MpObj bf_file_str();

static MpClass* file_class;

/**
 * get rest file length
 * @param fp file descripter
 * @return rest file length
 */
static long get_rest_size(FILE* fp){
    long cur, end;
    cur = ftell(fp);
    fseek(fp, 0, SEEK_END);
    end = ftell(fp);
    fseek(fp, cur, SEEK_SET);
    return end - cur;
}


static int is_file_obj(MpObj obj) {
    if (obj.type != TYPE_INSTANTCE) {
        return 0;
    }

    MpInstance* instance = GET_INSTANCE(obj);
    assert(instance != NULL);

    if (instance->klass != file_class) {
        return 0;
    }
    return 1;
}

MpObj bf_open() {
    // 打开文件函数，返回文件对象
    MpObj fpath = mp_take_str_obj_arg("fpath");
    MpObj mode = string_static("r+");
    if (mp_has_next_arg()) {
        mode = mp_take_str_obj_arg("mode");
    }
    FILE* fp = fopen(GET_CSTR(fpath), GET_CSTR(mode));
    if (fp == NULL) {
        mp_raise("open file failed");
    }

    if (file_class == NULL) {
        file_class = MpClass_New("fileobject", tm->builtins_mod);
        file_class->__str__ = mp_new_native_func_obj(tm->builtins_mod, bf_file_str);
        MpClass_setattr(file_class, string_const("write"), mp_new_native_func_obj(tm->builtins_mod, bf_file_write));
        MpClass_setattr(file_class, string_const("read"), mp_new_native_func_obj(tm->builtins_mod, bf_file_read));
        MpClass_setattr(file_class, string_const("close"), mp_new_native_func_obj(tm->builtins_mod, bf_file_close));
    }

    MpInstance* fileobject = class_instance(file_class);

    dict_set_by_cstr(fileobject->dict, "fp", mp_ptr_obj(fp));
    dict_set_by_cstr(fileobject->dict, "is_open", tm->_TRUE);
    dict_set_by_cstr(fileobject->dict, "fpath", fpath);
    dict_set_by_cstr(fileobject->dict, "mode", mode);

    return mp_to_obj(TYPE_INSTANTCE, fileobject);
}

static int is_file_open(MpInstance* data) {
    MpObj is_open = MpInstance_getattr(data, string_const("is_open"), NULL);
    return mp_is_true(is_open);
}

static void set_file_open(MpInstance* data, MpObj flag) {
    dict_set0(data->dict, string_const("is_open"), tm->_TRUE);
}

static FILE* get_file_ptr(MpInstance* data) {
    MpObj fp = MpInstance_getattr(data, string_const("fp"), NULL);
    if (MP_TYPE(fp) != TYPE_PTR) {
        mp_raise("expect fp type <ptr> but see %ot", fp);
    }
    return (FILE*) GET_PTR(fp);
}

static
MpObj bf_file_str() {
    MpObj self = mp_take_obj_arg("file.str");

    if (!is_file_obj(self)) {
        mp_raise("expect file object");
    }

    MpObj fpath = mp_getattr(self, string_const("fpath"));
    MpObj mode = mp_getattr(self, string_const("mode"));

    return mp_format("<fileobject fpath=%o mode=%o>", fpath, mode);
}

static 
MpObj bf_file_close() {
    MpObj self = mp_take_obj_arg("file_close: self");
    if (!is_file_obj(self)) {
        mp_raise("expect fileobject");
    }
    MpInstance* data = GET_INSTANCE(self);
    FILE* fp = get_file_ptr(data);
    if (is_file_open(data)) {
        int ret = fclose(fp);
        if (ret != 0) {
            mp_raise("close file failed");
        }
        set_file_open(data, tm->_FALSE);
    }
    return NONE_OBJECT;
}

static
MpObj bf_file_read() {
    MpObj self = mp_take_obj_arg("file_read: self");
    int len = -1;
    if (mp_has_next_arg()) {
        len = mp_take_int_arg("file_read: len");
    }
    if (!is_file_obj(self)) {
        mp_raise("expect fileobject");
    }
    MpInstance* data = GET_INSTANCE(self);
    FILE* fp = get_file_ptr(data);
    if (len == -1) {
        len = get_rest_size(fp);
    }
    if (is_file_open(data)) {
        MpObj text = string_alloc(NULL, len);
        char* s = GET_CSTR(text);
        int readsize = fread(s, 1, len, fp);
        return string_alloc(s, readsize);
    } else {
        mp_raise("file is not open");
    }
    return NONE_OBJECT;
}

static MpObj bf_file_write() {
    MpObj self = mp_take_obj_arg("file_write: self");
    MpStr* content = mp_take_str_ptr_arg("file_write: content");

    if (!is_file_obj(self)) {
        mp_raise("expect fileobject");
    }
    MpInstance* data = GET_INSTANCE(self);
    FILE* fp = get_file_ptr(data);
    if (is_file_open(data)) {
        fwrite(content->value, 1, content->len, fp);
        return NONE_OBJECT;
    } else {
        mp_raise("file is not open");
    }
    return NONE_OBJECT;
}

/**
 * load file
 * @param fpath file path
 * @return file text
 */
MpObj mp_load(char* fpath){
    FILE* fp = fopen(fpath, "rb");
    if(fp == NULL){
        mp_raise("load: can not open file \"%s\"", fpath);
        return NONE_OBJECT;
    }
    long len = get_rest_size(fp);
    if(len > MAX_FILE_SIZE){
        mp_raise("load: file too big to load, size = %d", len);
        return NONE_OBJECT;
    }
    MpObj text = string_alloc(NULL, len);
    char* s = GET_CSTR(text);
    int readsize = fread(s, 1, len, fp);
    if (readsize != len) {
        mp_raise("load: unexpected end of file");
    }
    fclose(fp);
    return text;
}

MpObj mp_save(char*fname, MpObj content) {
    FILE* fp = fopen(fname, "wb");
    if (fp == NULL) {
        mp_raise("mp_save: can not save to file \"%s\"", fname);
    }
    char* txt = GET_CSTR(content);
    int len = GET_STR_LEN(content);
    fwrite(txt, 1, len, fp);
    fclose(fp);
    return NONE_OBJECT;
}


MpObj bf_load(MpObj p){
    MpObj fname = mp_take_str_obj_arg("load");
    return mp_load(GET_CSTR(fname));
}

MpObj bf_save(){
    MpObj fname = mp_take_str_obj_arg("<save name>");
    return mp_save(GET_CSTR(fname), mp_take_str_obj_arg("<save content>"));
}


void mp_file_init() {
    mp_reg_builtin_func("save", bf_save);
    mp_reg_builtin_func("load", bf_load);
    mp_reg_builtin_func("open", bf_open);
}
