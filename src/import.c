/**
 * 模块导入功能
 * @author xupingmao
 * @since 2022/01/22
 * @modified 2022/01/22 00:22:02
 */
#include "include/mp.h"

MpObj obj_import(MpObj globals, MpObj name) {
	MpObj func = mp_get_global_by_cstr(globals, "_import");
	MpObj args[] = { globals, name };
	return obj_call_nargs(func, 2, args);
}

MpObj obj_import_attr(MpObj globals, MpObj module_name, MpObj attr) {
	MpObj func = mp_get_global_by_cstr(globals, "_import");
	MpObj args[] = { globals, module_name, attr };
	return obj_call_nargs(func, 3, args);
}