/** 
 * compile.c
 * 
 *  program := { statement }
 *  statement := assignment | def_statement | call_statement | class_statement
 *               | if_statement | while_statement | compare_statement | for_statement
 *               | import_statement | try_statement | del_statement 
 *               | global_statement | assert_statement | raise_statement
 *  assignment := attribute '=' right_value
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
 *  def_statement := 'def' name '(' arg_def_list ')' ':' statement_list
 *  class_statement := 'class' name '(' arg_def_list ')' ':' statement_list
 *  call_statement := attribute '(' arg_list ')'
 *  
 */

typedef struct _code {
    int op;
    int value;
} ByteCode;

typedef struct _token {
    int type;
    char* sval;
} Token;

typedef struct _scope {
    IntArray local_list;
    struct _scope prev;
    struct _scope next;
} Scope;

typedef struct _compiler {
    int length;
    int cap; // capacity
    int tag_id;
    Token token;
    Scope* scope;
    LexState* lex;
    ByteCode* codes;
} Compiler;

Compiler* compiler_new(char *text) {
    Compiler* c = tm_malloc(sizeof(Compiler));
    c->cap = 100;
    c->length = 0;
    c->codes = tm_malloc(sizeof(ByteCode) * c->cap);
    c->lex   = lex_new(text);
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

int emit_store(Compiler*c, int const_id) {
    if (is_local_var(c, const_id)) {
        int local_id = get_local_id(c, const_id);
        return emit_code(c, STORE_LOCAL, local_id);
    } else {
        return emit_code(c, STORE_GLOBAL, const_id);
    }
}

int is_local_var(Compiler*c, int const_id) {
    return 1;
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

static void next(Compiler* c) {
    lex_next(c->lex);
    if (c->lex->lookahead == 0) {
        c->token.type = EOP;
        c->token.sval = "";
        return;
    }
    c->token.type = c->lex->token_type;
    c->token.sval = c->lex->token_value;
}

void visit_program(char* text) {
    Compiler* c = compiler_new(text);
    next(c);
    visit_statement_list(c);
}

void visit_statement_list(Compiler*c) {
    while (1) {
        visit_statement(c);
    }
}

void visit_statement(Compiler* c) {
    switch (c->token.type) {
        case LEX_NAME  : visit_name(c); break;
        case LEX_IMPORT: visit_import(c); break;
        case LEX_FROM  : visit_from(c);   break;
        case LEX_GLOBAL: visit_global(c); break;
        case LEX_DEL   : visit_del(c);    break;
        case LEX_ASSERT: visit_assert(c); break;
        case LEX_RAISE : visit_raise(c);  break;

        case LEX_DEF   : visit_def(c); break;
        case LEX_CLASS : visit_class(c); break;

        case LEX_IF    : visit_if(c); break;
        case LEX_WHILE : visit_while(c); break;
        case LEX_FOR   : visit_for(c); break;
        case LEX_TRY   : visit_try(c); break;
        case ';'       : next(c); break;
    }
}

void visit_name(Compiler* c) {
    int name_id = get_const_id(c->token.sval);
    next(c);
    if (c->token.type == LEX_EQ) {
        next(c);
        visit_expr(c);
        emit_store(c, name_id);
    } else if (c->token.type == LEX_DOT) {
        emit_load(c, name_id);
        visit_attribute(c);
    }
}

void visit_assignment(Compiler* c) {
	visit_attribute(c, token);
	expect_next(c, LEX_EQ);
	visit_expr(c, token);
}

void visit_attribute(Compiler* c) {
    while (token->type == LEX_NAME) {

    }
}

char* get_op_string(int op) {
    switch(op) {
        case NEW_STRING: return "NEW_STRING";
    }
}

void test () {
    char* text = "name = 10";
    int i, codes_len;
    ByteCode* codes;
    Compiler* c = compiler_new(text);
    visit_program(c);

    codes = c->codes;
    codes_len = c->length;
    
    for (i = 0; i < codes_len; i++) {
        printf("%20s:%10d\n", get_op_string(codes[i].op), codes[i].value);
    }
}