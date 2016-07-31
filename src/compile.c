/** 
 * compile.c
 * 
 *  program := { statement }
 *  statement := assignment | def_statement | call_statement | class_statement
 *               | if_statement | while_statement | compare_statement | for_statement
 *               | import_statement | try_statement | del_statement 
 *               | global_statement | assert_statement | raise_statement
 *  assignment := left_value '=' right_value
 *  left_value := attribute { , attribute }
 *  attribute  := 'name' { ( '.' 'name' | '[' expr ']' ) }
 *  right_value := expr
 *  expr := 'not' ? expr
 *  expr := tuple_item ',' tuple_item
 *  tuple_item := or_item 'or' or_item
 *  or_item := and_item 'and' and_item
 *  and_item := cmp_item ( '>' | '<' | '>=' | '<=' | '==' | '!=' 
 *                | 'is' | 'is' 'not' | 'in' | 'not' 'in') cmp_item
 *  cmp_item := item ( '+' | '-') item
 *  item     := factor ( '*' | '/' ) factor
 *  factor   := name | attribute | number | string | list | dict | True | False | None 
 */

typedef struct _code {
    int op;
    int value;
} ByteCode;

typedef struct _compiler {
    ByteCode* codes;
    int length;
    int cap; // capacity
    int tag_id;
} Compiler;

Compiler* compiler_new() {
    Compiler* c = tm_malloc(sizeof(Compiler));
    c->cap = 100;
    c->length = 0;
    c->codes = tm_malloc(sizeof(ByteCode) * c->cap);
    tag_id = 0;
    return c;
}

void check_code_cap(Compiler* c) {
    // exactly, if should be c->length == c->cap
    if (c->length >= c->cap) {
        int old_cap = c->cap;
        c->cap += 1 + (c->cap / 2);
        c->codes = tm_realloc(c->codes, c->cap * sizeof(ByteCode));
    }
}

int emit_code(Compiler* c, int op, int value) {
    // return position
    check_code_cap(c);
    c->codes[c->length]->op = op;
    c->codes[c->length]->value = value;
    c->length += 1;
    return c->length - 1;
}

int emit_string(Compiler* c, char* sz) {
    Object str_obj = string_new(sz);
    int const_id = get_const_id(str_obj);
    return emit_code(c, LOAD_CONST, const_id);
}

int emit_number(Compiler* c, char* number) {
    Object num_obj = tm_number(number);
    int const_id = get_const_id(num_obj);
    return emit_code(c, LOAD_CONST, const_id);
}

int get_new_tag(Compiler* c) {
    c->tag_id += 1;
    return c->tag_id;
}

int emit_jump(Compiler* c, int tag) {
    // return tag position
    return emit_code(c, OP_JUMP, tag);
}

int back_patch_jump(Compiler*c, int position, int value) {
    c->codes[position]->value = value;
}

int emit_op(Compiler*c, int op) {
    return emit_code(c, op, 0);
}



