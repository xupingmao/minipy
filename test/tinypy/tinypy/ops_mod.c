/**
 * File: ops_mod
 * 主要是字符串格式化的功能，从minipy移植过来的
 */

#include <assert.h>
#include <ctype.h>

static tp_obj tp_char(TP, char c) {
    char buf[2];
    buf[0] = c;
    buf[1] = 0;
    return tp_string(buf);
}

#define MpObj tp_obj
#define TYPE_STR  TP_STRING
#define TYPE_LIST TP_LIST
#define TYPE_NUM TP_NUMBER

#define IS_STR(v)      ((v).type == TP_STRING)
#define MP_TYPE(v)     ((v).type)
#define GET_STR_LEN(v) ((v).string.len)
#define GET_CSTR(v)    ((v).string.val)
#define GET_STR_CHAR(v,i) ((v).string.val[i])
#define list_get(v,i) tp_get(tp, (v), tp_number((i)))
#define string_new(v) tp_string(v)
#define string_append_obj(result, value) result = tp_add(tp, result, tp_str(tp, value))
#define string_append_char(result, value) result = tp_add(tp, result, tp_char(tp, value))
#define string_append_cstr(result, value) result = tp_add(tp, result, tp_string(value))
#define mp_assert_type(value,t,msg) if ((value).type != (t)) { tp_raise(tp_None,tp_string(msg)); }

static tp_obj string_mod_list(TP, MpObj str, MpObj list) {
    assert(MP_TYPE(str)  == TYPE_STR);
    assert(MP_TYPE(list) == TYPE_LIST);

    const char* fmt = GET_CSTR(str);
    int str_length = GET_STR_LEN(str);
    int i = 0;
    int arg_index = 0;

    MpObj result = string_new("");
    tp_obj plist = list;

    for (i = 0; i < str_length; i++) {
        char c = fmt[i];

        if (c == '%') {
            i++;
            char num_buf[20];
            int num_len = 0;
            const char *fmt_temp = fmt;
            while (isdigit(fmt[i]) && num_len <= sizeof(num_buf)) {
                i++;
                num_len++;
            }

            if (num_len > 0) {
                strncpy(num_buf, fmt_temp, num_len);
            }

            if (num_len >= sizeof(num_buf)) {
                tp_raise(tp_None, tp_string("(tp_mod) format too long"));
            }

            /* TODO 处理 %03d 中间的数字部分 */

            switch(fmt[i]) {
                case 's':
                {
                    string_append_obj(result, list_get(plist, arg_index));
                    arg_index++;
                    break;
                }
                case 'd': 
                {
                    MpObj item = list_get(plist, arg_index);
                    mp_assert_type(item, TYPE_NUM, "obj_mod");
                    string_append_obj(result, item);
                    arg_index++;
                    break;
                }
                case 'r': 
                {
                    MpObj item = list_get(plist, arg_index);
                    if (IS_STR(item)) {
                        int j;
                        string_append_char(result, '\'');
                        for (j = 0; j < GET_STR_LEN(item); j++) {
                            char c1 = GET_STR_CHAR(item, j);
                            if (c1 == '\n') {
                                string_append_cstr(result, "\\n");
                            } else if (c1 == '\r') {
                                string_append_cstr(result, "\\r");
                            } else {
                                string_append_char(result, c1);
                            }
                        }
                        string_append_char(result, '\'');
                    } else {
                        string_append_obj(result, item);
                    }
                    arg_index++;
                    break;
                }
                default:
                    tp_raise(tp_None, tp_string("(obj_mod) unsupported format type"));
            }
        } else {
            string_append_char(result, c);
        }
    }

    return result;
}


tp_obj tp_string_mod(TP, tp_obj a, tp_obj b) {
    assert(a.type == TP_STRING);

    if (b.type == TP_LIST) {
        return string_mod_list(tp, a, b);
    } else {
        tp_obj list = tp_list(tp);
        tp_params_v(tp, 2, list, b);
        tp_append(tp);
        return string_mod_list(tp, a, list);
    }
}