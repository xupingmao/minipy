#include "vm.c"
#include <conio.h>

void gotoxy(int x, int y) 
{
    COORD pos;
    pos.X = x - 1;
    pos.Y = y - 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
}

Object bf_gotoxy() {
    const char* fnc = "gotoxy";
    int x = argTakeInt(fnc);
    int y = argTakeInt(fnc);
    gotoxy(x,y);
    return NONE_OBJECT;
}

Object bf_getch() {
    int key = getch();
    return tmNumber(key);
}

Object bf_ctime() {
    return tmNumber(time(NULL));
}

int winInit(int argc, char* argv[]) {
    int ret = vmInit(argc, argv);
    if (ret != 0) { 
        return ret;
    }
    /* use first frame */
    int code = setjmp(tm->frames->buf);
    if (code == 0) {
        loadBinary();
        regBuiltinFunc("gotoxy", bf_gotoxy);
        regBuiltinFunc("getch", bf_getch);
        regBuiltinFunc("ctime", bf_ctime);
        callModFunc("init", "boot");
    } else if (code == 1){
        traceback();
    } else if (code == 2){
        
    }
    vmDestroy();
    return 0;
}


int main(int argc, char *argv[])
{
    return winInit(argc, argv);
}