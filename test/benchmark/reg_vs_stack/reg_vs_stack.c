#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// 基于寄存器的VM和基于栈的VM对比
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

typedef struct _RegInst {
    int op;
    int a;
    int b;
    int c;
}RegInst;

typedef struct _StackInst {
    int op;
    int a;
} StackInst;


void print_locals(int* locals, int count) {
    int i = 0;
    for (i = 0; i < count; i++) {
        printf("%d,", locals[i]);
    }
    printf("\n");
}

int regTest (int loops) {
    int i = 0, j = 0;

    // 寄存器指令集
    RegInst instList[] = {
        {RSET, 0, 3, 0},  // locals[0] = 3
        {RSET, 1, 4, 0},  // locals[1] = 4
        {RMUL, 0, 1, 2},  // locals[0] = locals[1] * locals[2]
        {EOP,  0, 0, 0}
    };

    // instructions count
    int count = sizeof(instList) / sizeof(RegInst);
    // RegInst[] instList = {inst};
    int locals[] = {1,2,3,4,5,6,7,8,9};
    // define inst
    RegInst inst;

    for (i = 0; i < loops; i++) {
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

    print_locals(locals, 9);

    return 0;
}

int stackTest (int loops)
{
    int i = 0, j = 0;
    
    StackInst instList[] = {
        {LD_NUM, 3}, 
        {LD_NUM, 4},
        {MUL, 0},
        {ST_LOC, 0},
        {EOP, 0}
    };

    int locals[] = {1,2,3,4,5,6,7,8,9};
    int stack[]  = {0,0,0,0,0};
    int top = 0;
    
    // instructions count
    int count = 5;
    StackInst inst;

    for (i = 0; i < loops; i++) {
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

    print_locals(locals, 9);
    return 0;
}

typedef int (*TestFunc) (int);

void runWithProfile(TestFunc func, int loops, const char* message) {
    long t1 = clock();
    func(loops);
    long cost = clock() - t1;
    printf("%s:%ld\n", message, cost);
}


int main(int argc, char const *argv[])
{
    long t1, t2;
    int loops = 1000000;

    printf("loops: %d\n", loops);

    runWithProfile(regTest, loops, "regTest.clocks");
    runWithProfile(stackTest, loops, "stackTest.clocks");

    /*
    测试结果:(O2优化)
    loops: 1000000
    regTest.clocks:0
    stackTest.clocks:11

    测试结果:(无优化)
    loops: 1000000
    12,4,3,4,5,6,7,8,9,
    regTest.clocks:14
    12,2,3,4,5,6,7,8,9,
    stackTest.clocks:22

    15,28
    14,23
    13,23
    */

    return 0;
}