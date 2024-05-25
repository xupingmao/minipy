#include "../include/mp.h"

static MpObj bf_file_close();
static MpObj bf_file_read();
static MpObj bf_file_write();
static MpObj bf_file_get(MpData*, MpObj);
static MpObj bf_file_str(MpData*);


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
    if (obj.type != TYPE_DATA) {
        return 0;
    }
    MpData* data = GET_DATA(obj);
    if (!obj_eq_cstr(data->data_ptr[0], "fileobject")) {
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

    MpData* fileobject = data_new_ptr(5);
    fileobject->data_ptr[0] = string_static("fileobject"); // classname
    fileobject->data_ptr[1] = mp_ptr_obj(fp); // FILE ptr
    fileobject->data_ptr[2] = tm->_TRUE; // is_open
    fileobject->data_ptr[3] = fpath; // filename
    fileobject->data_ptr[4] = mode; // open mode
    fileobject->str = bf_file_str;
    fileobject->get = bf_file_get;

    return data_ptr_to_obj(fileobject);
}

static int is_file_open(MpData* data) {
    return is_true_obj(data->data_ptr[2]);
}

static void set_file_open(MpData* data, MpObj flag) {
    data->data_ptr[2] = flag;
}

static FILE* get_file_ptr(MpData* data) {
    return data->data_ptr[1].value.ptr;
}

static
MpObj bf_file_get(MpData* data, MpObj attr) {
    MpObj self = data_ptr_to_obj(data);
    if (!is_file_obj(self)) {
        mp_raise("expect file object");
    }
    if (obj_eq_cstr(attr, "close")) {
        return func_new(tm->builtins_mod, self, bf_file_close);
    }
    if (obj_eq_cstr(attr, "read")) {
        return func_new(tm->builtins_mod, self, bf_file_read);
    }
    if (obj_eq_cstr(attr, "write")) {
        return func_new(tm->builtins_mod, self, bf_file_write);
    }
    mp_raise("AttributeError: 'fileobject' object has no attribute %o", attr);
    return NONE_OBJECT;
}

static
MpObj bf_file_str(MpData* data) {
    MpObj self = data_ptr_to_obj(data);
    if (!is_file_obj(self)) {
        mp_raise("expect file object");
    }
    return mp_format("<fileobject name=%o mode=%o>", data->data_ptr[3], data->data_ptr[4]);
}

static 
MpObj bf_file_close() {
    MpObj self = mp_take_obj_arg("file_close: self");
    if (!is_file_obj(self)) {
        mp_raise("expect fileobject");
    }
    MpData* data = GET_DATA(self);
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
    MpData* data = GET_DATA(self);
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
    MpData* data = GET_DATA(self);
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
    reg_builtin_func("save", bf_save);
    reg_builtin_func("load", bf_load);
    reg_builtin_func("open", bf_open);
}
