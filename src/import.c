/**
 * 模块导入功能
 * @author xupingmao
 * @since 2022/01/22
 * @modified 2022/01/22 00:22:02
 */
#include "include/mp.h"

MpObj obj_import(MpObj globals, MpObj name) {
	int old_gc_state = tm->gc_state;
	tm->gc_state = 0; // disable gc
	MpObj func = mp_get_global_by_cstr(globals, "_import");
	MpObj args[] = { globals, name };
	MpObj result = mp_call_with_nargs(func, 2, args);
	tm->gc_state = old_gc_state; // restore gc state;
	return result;
}

MpObj obj_import_attr(MpObj globals, MpObj module_name, MpObj attr) {
	int old_gc_state = tm->gc_state;
	tm->gc_state = 0; // disable gc
	MpObj func = mp_get_global_by_cstr(globals, "_import");
	MpObj args[] = { globals, module_name, attr };
	MpObj result = mp_call_with_nargs(func, 3, args);
	tm->gc_state = old_gc_state; // restore gc state;
	return result;
}