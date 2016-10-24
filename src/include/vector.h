/**
 * define vector structure
 * DEF_VECTOR will generate
 *  - vector Structure
 *  - vector new/free
 *  - vector get/set/push/pop
 * @since 2016-08-21
 */
#define DEF_VECTOR(VectorType, ValType)                                            \
typedef struct t##VectorType {                                                     \
    int len;                                                                       \
    int cap;                                                                       \
    ValType* values;                                                               \
} VectorType;                                                                      \
VectorType* VectorType##_new() {                                                   \
    VectorType* vector = malloc(sizeof(VectorType));                               \
    vector->len = 0;                                                               \
    vector->cap = 5;                                                               \
    vector->values = malloc(sizeof(ValType) * vector->cap);                        \
    return vector;                                                                 \
}                                                                                  \
void VectorType##_free(VectorType* vector) {                                       \
    free(vector->values);                                                          \
    free(vector);                                                                  \
}                                                                                  \
ValType* VectorType##_get(VectorType* vector, int i) {                             \
    if (i < 0) i += vector->len;                                                   \
    if (i < 0 || i >= vector->len) {                                               \
        return NULL;                                                               \
    } else {                                                                       \
        return &vector->values[i];                                                 \
    }                                                                              \
}                                                                                  \
void VectorType##_checksize(VectorType* vector)  {                                 \
    if (vector->len == vector->cap) {                                              \
        int oldcap = vector->cap;                                                  \
        vector->cap += 10;                                                         \
        vector->values = realloc(vector->values, sizeof(ValType) * vector->cap);   \
    }                                                                              \
}                                                                                  \
int VectorType##_set(VectorType* vector, int i, ValType value) {                   \
    if (i < 0) i += vector->len;                                                   \
    if (i < 0 || i >= vector->len) {                                               \
        return 0;                                                                  \
    } else {                                                                       \
        vector->values[i] = value;                                                 \
        return 1;                                                                  \
    }                                                                              \
}                                                                                  \
                                                                                   \
void VectorType##_push(VectorType* vector, ValType value) {                        \
    VectorType##_checksize(vector);                                                \
    vector->values[vector->len] = value;                                           \
    vector->len++;                                                                 \
}                                                                                  \
                                                                                   \
ValType* VectorType##_pop(VectorType* vector) {                                    \
    return VectorType##_get(vector, -1);                                           \
}                                                                                  
