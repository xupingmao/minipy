#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define N 1000000

// calculate `a = 3 * 4` by register based vm and stack based vm

enum OP {
    /* register based instructions */
    RSET,
    RADD,
    RMUL,
    RRET,

    /* stack based instruction */
    LD_NUM,
    LD_LOC,
    ST_LOC,
    ADD,
    MUL,
    RET,

    /* end of program */
    EOP, 
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
    int i = 0, j = 0;
    RegInst *instList = malloc(sizeof(RegInst) * 10);
    instList[0] = (RegInst){RSET, 0, 3, 0};
    instList[1] = (RegInst){RSET, 1, 4, 0};
    instList[2] = (RegInst){RMUL, 0, 1, 2};
    instList[3] = (RegInst){EOP,  0, 0, 0};

    // instructions count
    int count = 4;
    // RegInst[] instList = {inst};
    int locals[] = {1,2,3,4,5,6,7,8,9};
    // define inst
    RegInst inst;

    for (i = 0; i < N; i++) {
        for (j = 0; j < count; j++) {
            inst = instList[j];
            switch(inst.op) {
                case RSET:
                    locals[inst.a] = inst.b;
                    break;
                case RADD:
                    locals[inst.a] = locals[inst.b] + locals[inst.b]; 
                    break;
                case RMUL:
                    locals[inst.a] = locals[inst.b] * locals[inst.c];
                    break;
                case RRET:
                    return 0;
                case EOP:
                    break;
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
    
    StackInst *instList = malloc(sizeof(StackInst) * 10);
    instList[0] = (StackInst){LD_NUM, 3};
    instList[1] = (StackInst){LD_NUM, 4};
    instList[2] = (StackInst){MUL, 0};
    instList[3] = (StackInst){ST_LOC, 0};
    instList[4] = (StackInst){EOP, 0};
    StackInst inst;

    // RegInst[] instList = {inst};
    int locals[] = {1,2,3,4,5,6,7,8,9};
    int stack[]  = {0,0,0,0,0};
    int top = 0;
    
    // instructions count
    int count = 5;

    for (i = 0; i < N; i++) {
        top = 0;
        for (j = 0; j < count; j++) {
            inst = instList[j];
            switch(inst.op) {
                case LD_NUM:
                    top ++;
                    stack[top] = inst.a;
                    break;
                case LD_LOC:
                    top ++;
                    stack[top] = locals[inst.a];
                    break;
                case ST_LOC:
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
                case EOP:
                    top = 0;
                    break;
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