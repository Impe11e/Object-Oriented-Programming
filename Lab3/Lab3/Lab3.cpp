#include "framework.h"
#include "Lab3.h"
#include "shape_editor.h"
#include <commctrl.h>
#pragma comment(lib, "Comctl32.lib")

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
ShapeObjectsEditor editor;
HWND hToolbar = NULL;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    InitCommonControls();

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB3));
    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB3));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB3);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   hToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
       WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_TOP | TBSTYLE_TOOLTIPS | TBSTYLE_WRAPABLE,
       0, 0, 0, 0,
       hWnd, (HMENU)1000, hInstance, NULL);

   if (hToolbar)
   {
       SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
       
       HIMAGELIST hImageList = ImageList_Create(24, 24, ILC_COLOR32 | ILC_MASK, 4, 0);
       HBITMAP hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TOOLBAR));
       
       if (hBitmap) {
           ImageList_Add(hImageList, hBitmap, NULL);
           DeleteObject(hBitmap);
       }
       
       SendMessage(hToolbar, TB_SETIMAGELIST, 0, (LPARAM)hImageList);
       
       TBBUTTON tbb[4];
       ZeroMemory(tbb, sizeof(tbb));
       
       tbb[0].iBitmap = 0;
       tbb[0].fsState = TBSTATE_ENABLED;
       tbb[0].fsStyle = TBSTYLE_BUTTON;
       tbb[0].idCommand = ID_POINT;
       
       tbb[1].iBitmap = 1;
       tbb[1].fsState = TBSTATE_ENABLED;
       tbb[1].fsStyle = TBSTYLE_BUTTON;
       tbb[1].idCommand = ID_LINE;
       
       tbb[2].iBitmap = 2;
       tbb[2].fsState = TBSTATE_ENABLED;
       tbb[2].fsStyle = TBSTYLE_BUTTON;
       tbb[2].idCommand = ID_RECTANGLE;
       
       tbb[3].iBitmap = 3;
       tbb[3].fsState = TBSTATE_ENABLED;
       tbb[3].fsStyle = TBSTYLE_BUTTON;
       tbb[3].idCommand = ID_ELLIPSE;
       
       SendMessage(hToolbar, TB_ADDBUTTONS, 4, (LPARAM)&tbb);
       SendMessage(hToolbar, TB_AUTOSIZE, 0, 0);
   }

   editor.UpdateWindowTitle(hWnd, szTitle);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:
        editor.OnSize(hWnd, hToolbar);
        break;
    case WM_INITMENUPOPUP:
        editor.OnInitMenuPopup(hWnd, wParam);
        break;
    case WM_NOTIFY:
        editor.OnNotify(hWnd, wParam, lParam);
        break;
    case WM_LBUTTONDOWN:
        editor.OnLBdown(hWnd);
        break;
    case WM_LBUTTONUP:
        editor.OnLBup(hWnd);
        break;
    case WM_MOUSEMOVE:
        editor.OnMouseMove(hWnd);
        break;
    case WM_PAINT:
        editor.OnPaint(hWnd);
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case ID_POINT:
            case ID_LINE:
            case ID_RECTANGLE:
            case ID_ELLIPSE:
                editor.OnToolButton(hWnd, hToolbar, wmId);
                editor.UpdateWindowTitle(hWnd, szTitle);
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
