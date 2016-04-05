#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define N 1000000

enum OP {
    RADD,
    RRET,
    RMUL,
    LDL,
    STL,
    ADD,
    MUL,
    RET
};

typedef struct _RegInst
{
    int op;
    int a;
    int b;
    int c;
}RegInst;

typedef struct _StackInst 
{
    int op;
    int a;
} StackInst;

int regTest ()
{
    RegInst inst = {RMUL, 0, 3, 2};
    int i = 0, j = 0;
    int count = 1;
    RegInst *instList = malloc(sizeof(RegInst) * 10);
    instList[0] = inst;

    // RegInst[] instList = {inst};
    int locals[] = {1,2,3,4,5,6,7,8,9};

    for (i = 0; i < N; i++) {
        for (j = 0; j < count; j++) {
            inst = instList[j];
            switch(inst.op) {
            case RADD:
                locals[inst.a] = locals[inst.b]+locals[inst.b]; 
                break;
            case RMUL:
                locals[inst.a] = locals[inst.b] * locals[inst.c];
                break;
            case RRET:
                return 0;
                // return locals[inst.a];
            }
        }
    }
    free(instList);
    return 0;
}

int stackTest ()
{
    int i = 0, j = 0;
    int count = 1;
    StackInst *instList = malloc(sizeof(StackInst) * 10);
    instList[0] = (StackInst){LDL, 3};
    instList[1] = (StackInst){LDL, 2};
    instList[2] = (StackInst){MUL, 0};
    instList[3] = (StackInst){STL, 0};
    StackInst inst;

    // RegInst[] instList = {inst};
    int locals[] = {1,2,3,4,5,6,7,8,9};
    int stack[] = {0,0,0,0,0};
    int top = 0;
    count = 4;

    for (i = 0; i < N; i++) {
        for (j = 0; j < count; j++) {
            top = 0;
            inst = instList[j];
            switch(inst.op) {
            case LDL:
                top ++;
                stack[top] = locals[inst.a];
                break;
            case STL:
                locals[inst.a] = stack[top];
                top--;
                break;
            case ADD:
            {
                int a = stack[top];
                top--;
                int b = stack[top];
                stack[top] = a + b;
                break;
            }
            case MUL:
            {
                int a = stack[top];
                top--;
                int b = stack[top];
                stack[top] = a * b;
                break;
            }
            }
        }
    }
    free(instList);
    return 0;
}

int main(int argc, char const *argv[])
{
    long t1, t2;
    t1 = clock();
    regTest();
    t2 = clock();
    printf("regTest=%ld\n", t2-t1);

    t1 = clock();
    stackTest();
    t2 = clock();
    printf("stackTest=%ld\n", t2-t1);
    return 0;
}