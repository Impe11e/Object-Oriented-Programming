#include "framework.h"
#include "Lab2.h"
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

void UpdateWindowTitle(HWND hWnd);
void UpdateToolbarButtons(int selectedButton);
void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnSize(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    InitCommonControls();

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2));
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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB2));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB2);
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
   
   hToolbar = CreateToolbarEx(hWnd,
       WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS | CCS_TOP | TBSTYLE_TOOLTIPS,
       1000,
       4,
       hInstance,
       IDB_TOOLBAR,
       tbb,
       4,
       24, 24, 24, 24,
       sizeof(TBBUTTON));

   UpdateWindowTitle(hWnd);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void UpdateWindowTitle(HWND hWnd)
{
    WCHAR title[256];
    WCHAR baseTitle[MAX_LOADSTRING];
    LoadStringW(hInst, IDS_APP_TITLE, baseTitle, MAX_LOADSTRING);
    
    const WCHAR* shapeName = editor.GetCurrentShapeName();
    wcscpy_s(title, shapeName);
    wcscat_s(title, baseTitle);
    
    SetWindowTextW(hWnd, title);
}

void UpdateToolbarButtons(int selectedButton)
{
    if (!hToolbar) return;
    
    int buttons[] = {ID_POINT, ID_LINE, ID_RECTANGLE, ID_ELLIPSE};
    for (int i = 0; i < 4; i++) {
        SendMessage(hToolbar, TB_CHECKBUTTON, buttons[i], MAKELONG(buttons[i] == selectedButton, 0));
    }
}

void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR pnmh = (LPNMHDR)lParam;
    if (pnmh->code == TTN_NEEDTEXT)
    {
        LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
        switch (lpttt->hdr.idFrom)
        {
        case ID_POINT:
            lstrcpy(lpttt->szText, L"Крапка");
            break;
        case ID_LINE:
            lstrcpy(lpttt->szText, L"Лінія");
            break;
        case ID_RECTANGLE:
            lstrcpy(lpttt->szText, L"Прямокутник");
            break;
        case ID_ELLIPSE:
            lstrcpy(lpttt->szText, L"Еліпс");
            break;
        default:
            lstrcpy(lpttt->szText, L"Щось невідоме");
        }
    }
}

void OnSize(HWND hWnd)
{
    RECT rc, rw;
    if (hToolbar)
    {
        GetClientRect(hWnd, &rc);
        GetWindowRect(hToolbar, &rw);
        
        ScreenToClient(hWnd, (POINT*)&rw);
        ScreenToClient(hWnd, ((POINT*)&rw) + 1);
        
        MoveWindow(hToolbar, 0, 0,
            rc.right - rc.left,
            rw.bottom - rw.top, TRUE);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE: 
        if (hToolbar) {
            SendMessage(hToolbar, TB_AUTOSIZE, 0, 0);
        }
        break;
    case WM_INITMENUPOPUP:
        editor.OnInitMenuPopup(hWnd, wParam);
        break;
    case WM_NOTIFY: 
        OnNotify(hWnd, wParam, lParam);
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
                editor.StartPointEditor();
                UpdateWindowTitle(hWnd);
                UpdateToolbarButtons(ID_POINT);
                break;
            case ID_LINE:
                editor.StartLineEditor();
                UpdateWindowTitle(hWnd);
                UpdateToolbarButtons(ID_LINE);
                break;
            case ID_RECTANGLE:
                editor.StartRectEditor();
                UpdateWindowTitle(hWnd);
                UpdateToolbarButtons(ID_RECTANGLE);
                break;
            case ID_ELLIPSE:
                editor.StartEllipseEditor();
                UpdateWindowTitle(hWnd);
                UpdateToolbarButtons(ID_ELLIPSE);
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
