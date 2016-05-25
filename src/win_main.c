#include "vm.c"
#include <conio.h>
#include <windows.h>

Object g_repaint_func;
HWND   g_hwnd;
// HANDLE std_input = Get_std_handle(STD_INPUT_HANDLE);
// HANDLE std_output = Get_std_handle(STD_OUTPUT_HANDLE);

void gotoxy(int x, int y) 
{
    COORD pos;
    pos.X = x - 1;
    pos.Y = y - 1;
    Set_console_cursor_position(Get_std_handle(STD_OUTPUT_HANDLE),pos);
}

Object bf_gotoxy() {
    const char* fnc = "gotoxy";
    int x = arg_take_int(fnc);
    int y = arg_take_int(fnc);
    gotoxy(x,y);
    return NONE_OBJECT;
}

Object bf_getch() {
    int key = getch();
    return tm_number(key);
}

Object bf_ctime() {
    return tm_number(time(NULL));
}


Object bf_set_repaint_func() {
    Object func = arg_take_func_obj("set_repaint_func");
    g_repaint_func = func;
    return NONE_OBJECT;
}


Object bf__draw_text() {
  char* text = arg_take_sz("Draw_text");
  PAINTSTRUCT ps;
  HDC hdc;
  RECT rc;

  HWND hwnd = g_hwnd;
  hdc = Begin_paint(hwnd, &ps);

  Get_client_rect(hwnd, &rc);
  Set_text_color(hdc, RGB(240,240,96));
  Set_bk_mode(hdc, TRANSPARENT);
  Draw_text(hdc, text, -1, &rc, DT_CENTER|DT_SINGLELINE|DT_VCENTER); 
  End_paint(hwnd, &ps);
  return NONE_OBJECT;
}

#define APPNAME "HELLO_WIN"

char sz_app_name[] = APPNAME; // The name of this application
char sz_title[] = APPNAME; // The title bar text
char *p_window_text;

HINSTANCE g_h_inst; // current instance

void Center_window(HWND h_wnd);

//+---------------------------------------------------------------------------
//
// Function: Wnd_proc
//
// Synopsis: very unusual type of function - gets called by system to
// process windows messages.
//
// Arguments: same as always.
//----------------------------------------------------------------------------

LRESULT CALLBACK Wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
 switch (message)
 {
  // ----------------------- first and last
  case WM_CREATE:
   Center_window(hwnd);
   break;

  case WM_DESTROY:
   Post_quit_message(0);
   break;


  // ----------------------- get out of it...
  case WM_RBUTTONUP:
   Destroy_window(hwnd);
   break;

  case WM_KEYDOWN:
   if (VK_ESCAPE == w_param)
    Destroy_window(hwnd);
   break;


  // ----------------------- display our minimal info
  case WM_PAINT:
  {
   PAINTSTRUCT ps;
   HDC hdc;
   RECT rc;
   // hdc = Begin_paint(hwnd, &ps);

   // Get_client_rect(hwnd, &rc);
   // Set_text_color(hdc, RGB(240,240,96));
   // Set_bk_mode(hdc, TRANSPARENT);
   // Draw_text(hdc, p_window_text, -1, &rc, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
   // int i;
   // for(i = 0; i < 100; i++) {
   //  Set_pixel(hdc, 10 + i, 10, RGB(100,100,100));
   // }
   tm_println(g_repaint_func);
   call_function(g_repaint_func);
   // End_paint(hwnd, &ps);
   break;
  }

  // ----------------------- let windows do all other stuff
  default:
   return Def_window_proc(hwnd, message, w_param, l_param);
 }
 return 0;
}

//+---------------------------------------------------------------------------
//
// Function: Win_main
//
// Synopsis: standard entrypoint for GUI Win32 apps
//
//----------------------------------------------------------------------------
int APIENTRY Win_main(
    HINSTANCE h_instance,
    HINSTANCE h_prev_instance,
    LPSTR lp_cmd_line,
    int n_cmd_show)
{
    MSG msg;

    WNDCLASS wc;

    HWND hwnd;

    // Fill in window class structure with parameters that describe
    // the main window.

    Zero_memory(&wc, sizeof wc);
    wc.h_instance = h_instance;
    wc.lpsz_class_name = sz_app_name;
    wc.lpfn_wnd_proc = (WNDPROC)Wnd_proc;
    wc.style = CS_DBLCLKS|CS_VREDRAW|CS_HREDRAW;
    wc.hbr_background = (HBRUSH)Get_stock_object(BLACK_BRUSH);
    wc.h_icon = Load_icon(NULL, IDI_APPLICATION);
    wc.h_cursor = Load_cursor(NULL, IDC_ARROW);

    if (FALSE == Register_class(&wc)) return 0;


    // create the browser
    hwnd = Create_window(
        sz_app_name,
        sz_title,
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        360,//CW_USEDEFAULT,
        240,//CW_USEDEFAULT,
        0,
        0,
        g_h_inst,
        0);

    if (NULL == hwnd) return 0;

    g_hwnd = hwnd;

    printf("lp_cmd_line=%s\n", lp_cmd_line);
    p_window_text = lp_cmd_line[0] ? lp_cmd_line : "Hello Windows!";

    g_repaint_func.type = 0;

    printf("load file %s\n", p_window_text);

    // start vm.
    char *argv[] = {"win_main.exe", lp_cmd_line};
    int ret = vm_init(2, argv);
    if (ret != 0) { 
        return ret;
    }
    /* use first frame */
    int code = setjmp(tm->frames->buf);
    if (code == 0) {
        load_binary();
        reg_builtin_func("gotoxy", bf_gotoxy);
        reg_builtin_func("getch", bf_getch);
        reg_builtin_func("ctime", bf_ctime);
        reg_builtin_func("set_repaint_func", bf_set_repaint_func);
        reg_builtin_func("Draw_text", bf__draw_text);
        call_mod_func("init", "boot");
    } else if (code == 1){
        traceback();
    } else if (code == 2){
        
    }

    // Main message loop:
    while (Get_message(&msg, NULL, 0, 0) > 0)
    {
        Translate_message(&msg);
        Dispatch_message(&msg);
    }

    vm_destroy();

    return msg.w_param;
}

//+---------------------------------------------------------------------------

//+---------------------------------------------------------------------------

void Center_window(HWND hwnd_self)
{
 RECT rw_self, rc_parent, rw_parent; HWND hwnd_parent;
 hwnd_parent = Get_parent(hwnd_self);
 if (NULL==hwnd_parent) hwnd_parent = Get_desktop_window();
 Get_window_rect(hwnd_parent, &rw_parent);
 Get_client_rect(hwnd_parent, &rc_parent);
 Get_window_rect(hwnd_self, &rw_self);
 Set_window_pos(hwnd_self, NULL,
  rw_parent.left + (rc_parent.right + rw_self.left - rw_self.right) / 2,
  rw_parent.top + (rc_parent.bottom + rw_self.top - rw_self.bottom) / 2,
  0, 0,
  SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE
  );
}













