#include "vm.c"
#include <conio.h>
#include <windows.h>

Object g_repaintFunc;
HWND   g_hwnd;
// HANDLE stdInput = GetStdHandle(STD_INPUT_HANDLE);
// HANDLE stdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

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


Object bf_setRepaintFunc() {
    Object func = argTakeFuncObj("setRepaintFunc");
    g_repaintFunc = func;
    return NONE_OBJECT;
}


Object bf_DrawText() {
  char* text = argTakeSz("DrawText");
  PAINTSTRUCT ps;
  HDC hdc;
  RECT rc;

  HWND hwnd = g_hwnd;
  hdc = BeginPaint(hwnd, &ps);

  GetClientRect(hwnd, &rc);
  SetTextColor(hdc, RGB(240,240,96));
  SetBkMode(hdc, TRANSPARENT);
  DrawText(hdc, text, -1, &rc, DT_CENTER|DT_SINGLELINE|DT_VCENTER); 
  EndPaint(hwnd, &ps);
  return NONE_OBJECT;
}

#define APPNAME "HELLO_WIN"

char szAppName[] = APPNAME; // The name of this application
char szTitle[] = APPNAME; // The title bar text
char *pWindowText;

HINSTANCE g_hInst; // current instance

void CenterWindow(HWND hWnd);

//+---------------------------------------------------------------------------
//
// Function: WndProc
//
// Synopsis: very unusual type of function - gets called by system to
// process windows messages.
//
// Arguments: same as always.
//----------------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
 switch (message)
 {
  // ----------------------- first and last
  case WM_CREATE:
   CenterWindow(hwnd);
   break;

  case WM_DESTROY:
   PostQuitMessage(0);
   break;


  // ----------------------- get out of it...
  case WM_RBUTTONUP:
   DestroyWindow(hwnd);
   break;

  case WM_KEYDOWN:
   if (VK_ESCAPE == wParam)
    DestroyWindow(hwnd);
   break;


  // ----------------------- display our minimal info
  case WM_PAINT:
  {
   PAINTSTRUCT ps;
   HDC hdc;
   RECT rc;
   // hdc = BeginPaint(hwnd, &ps);

   // GetClientRect(hwnd, &rc);
   // SetTextColor(hdc, RGB(240,240,96));
   // SetBkMode(hdc, TRANSPARENT);
   // DrawText(hdc, pWindowText, -1, &rc, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
   // int i;
   // for(i = 0; i < 100; i++) {
   //  SetPixel(hdc, 10 + i, 10, RGB(100,100,100));
   // }
   tmPrintln(g_repaintFunc);
   callFunction(g_repaintFunc);
   // EndPaint(hwnd, &ps);
   break;
  }

  // ----------------------- let windows do all other stuff
  default:
   return DefWindowProc(hwnd, message, wParam, lParam);
 }
 return 0;
}

//+---------------------------------------------------------------------------
//
// Function: WinMain
//
// Synopsis: standard entrypoint for GUI Win32 apps
//
//----------------------------------------------------------------------------
int APIENTRY WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    MSG msg;

    WNDCLASS wc;

    HWND hwnd;

    // Fill in window class structure with parameters that describe
    // the main window.

    ZeroMemory(&wc, sizeof wc);
    wc.hInstance = hInstance;
    wc.lpszClassName = szAppName;
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.style = CS_DBLCLKS|CS_VREDRAW|CS_HREDRAW;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (FALSE == RegisterClass(&wc)) return 0;


    // create the browser
    hwnd = CreateWindow(
        szAppName,
        szTitle,
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        360,//CW_USEDEFAULT,
        240,//CW_USEDEFAULT,
        0,
        0,
        g_hInst,
        0);

    if (NULL == hwnd) return 0;

    g_hwnd = hwnd;

    printf("lpCmdLine=%s\n", lpCmdLine);
    pWindowText = lpCmdLine[0] ? lpCmdLine : "Hello Windows!";

    g_repaintFunc.type = 0;

    printf("load file %s\n", pWindowText);

    // start vm.
    char *argv[] = {"win_main.exe", lpCmdLine};
    int ret = vmInit(2, argv);
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
        regBuiltinFunc("setRepaintFunc", bf_setRepaintFunc);
        regBuiltinFunc("DrawText", bf_DrawText);
        callModFunc("init", "boot");
    } else if (code == 1){
        traceback();
    } else if (code == 2){
        
    }

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    vmDestroy();

    return msg.wParam;
}

//+---------------------------------------------------------------------------

//+---------------------------------------------------------------------------

void CenterWindow(HWND hwnd_self)
{
 RECT rw_self, rc_parent, rw_parent; HWND hwnd_parent;
 hwnd_parent = GetParent(hwnd_self);
 if (NULL==hwnd_parent) hwnd_parent = GetDesktopWindow();
 GetWindowRect(hwnd_parent, &rw_parent);
 GetClientRect(hwnd_parent, &rc_parent);
 GetWindowRect(hwnd_self, &rw_self);
 SetWindowPos(hwnd_self, NULL,
  rw_parent.left + (rc_parent.right + rw_self.left - rw_self.right) / 2,
  rw_parent.top + (rc_parent.bottom + rw_self.top - rw_self.bottom) / 2,
  0, 0,
  SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE
  );
}

