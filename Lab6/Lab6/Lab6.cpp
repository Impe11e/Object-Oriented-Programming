#include "framework.h"
#include "Lab6.h"
#include <string>
#include <vector>
#include <sstream>
#include <shellapi.h>
#include <fstream>

#define MAX_LOADSTRING 100

#define WM_APP_OBJECT2_READY    (WM_USER + 1)
#define WM_APP_OBJECT3_READY    (WM_USER + 2)
#define WM_APP_DATA_GENERATED   (WM_USER + 3)
#define APP_SEND_PARAMS         (WM_APP + 1)
#define IDM_PROCESS_DATA        (WM_USER + 4)
#define WM_APP_TOO_MANY_POINTS  (WM_USER + 5)

HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                
WCHAR szWindowClass[MAX_LOADSTRING];          
HWND g_hWndObject2 = NULL;
HWND g_hWndObject3 = NULL;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                MyInitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

static void Lab6Log(const std::wstring &s)
{
    wchar_t modulePath[MAX_PATH];
    if (GetModuleFileNameW(NULL, modulePath, MAX_PATH) == 0) return;
    std::wstring path(modulePath);
    size_t pos = path.find_last_of(L"\\/");
    std::wstring dir = (pos == std::wstring::npos) ? L"." : path.substr(0, pos+1);
    std::wstring log = dir + L"lab6_debug.txt";
    std::wofstream ofs(log, std::ios::app);
    if (ofs) ofs << s << std::endl;
}

int SendCopyData(HWND hWndDest, HWND hWndSrc, void *lp, long cb)
{
    Lab6Log(L"SendCopyData called");
    COPYDATASTRUCT cds;
    cds.dwData = 1;
    cds.cbData = cb;
    cds.lpData = lp;
    int res = (int)SendMessage(hWndDest, WM_COPYDATA, (WPARAM)hWndSrc, (LPARAM)&cds);
    std::wostringstream ss; ss << L"SendCopyData result=" << res; Lab6Log(ss.str());
    return res;
}

bool LaunchExeWithArgs(const std::wstring &exeName, const std::wstring &args)
{
    Lab6Log(std::wstring(L"LaunchExeWithArgs: ") + exeName + L" args=" + args);
    wchar_t modulePath[MAX_PATH];
    if (GetModuleFileNameW(NULL, modulePath, MAX_PATH) == 0) return false;
    std::wstring path(modulePath);
    size_t pos = path.find_last_of(L"\\/");
    std::wstring dir = (pos == std::wstring::npos) ? L"." : path.substr(0, pos+1);

    std::vector<std::wstring> candidates;
    candidates.push_back(dir + exeName);
    candidates.push_back(dir + L"..\\Object2\\Debug\\" + exeName);
    candidates.push_back(dir + L"..\\Object2\\Release\\" + exeName);
    candidates.push_back(dir + L"..\\Object3\\Debug\\" + exeName);
    candidates.push_back(dir + L"..\\Object3\\Release\\" + exeName);
    candidates.push_back(dir + L"..\\Debug\\" + exeName);
    candidates.push_back(dir + L"..\\Release\\" + exeName);

    for (const auto &full : candidates)
    {
        std::wstring cmd = L"\"" + full + L"\"";
        if (!args.empty()) cmd += L" " + args;
        std::vector<wchar_t> cmdbuf(cmd.begin(), cmd.end()); cmdbuf.push_back(0);
        STARTUPINFOW si = {0}; si.cb = sizeof(si);
        PROCESS_INFORMATION pi = {0};
        if (CreateProcessW(NULL, cmdbuf.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
        {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
            Lab6Log(std::wstring(L"CreateProcess OK: ") + full);
            return true;
        }
        Lab6Log(std::wstring(L"CreateProcess failed, try ShellExecute: ") + full);
        HINSTANCE h = ShellExecuteW(NULL, L"open", full.c_str(), args.empty() ? NULL : args.c_str(), NULL, SW_SHOWNORMAL);
        if ((INT_PTR)h > 32) { Lab6Log(std::wstring(L"ShellExecute OK: ") + full); return true; }
    }

    std::wstring cmd = L"\"" + exeName + L"\"";
    if (!args.empty()) cmd += L" " + args;
    std::vector<wchar_t> cmdbuf(cmd.begin(), cmd.end()); cmdbuf.push_back(0);
    STARTUPINFOW si = {0}; si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {0};
    if (CreateProcessW(NULL, cmdbuf.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        Lab6Log(std::wstring(L"CreateProcess OK (fallback): ") + exeName);
        return true;
    }
    HINSTANCE h = ShellExecuteW(NULL, L"open", exeName.c_str(), args.empty() ? NULL : args.c_str(), NULL, SW_SHOWNORMAL);
    if ((INT_PTR)h > 32) { Lab6Log(std::wstring(L"ShellExecute OK (fallback): ") + exeName); return true; }

    Lab6Log(L"LaunchExeWithArgs failed");
    return false;
}

static void ArrangeWindows(HWND hMain)
{
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    int topHeight = screenH / 3;
    int bottomHeight = screenH - topHeight;
    int leftWidth = screenW / 2;
    int rightWidth = screenW - leftWidth;

    SetWindowPos(hMain, HWND_TOP, 0, 0, leftWidth, topHeight, SWP_SHOWWINDOW);

    if (g_hWndObject2 && IsWindow(g_hWndObject2))
    {
        SetWindowPos(g_hWndObject2, HWND_TOP, leftWidth, 0, rightWidth, topHeight, SWP_SHOWWINDOW);
    }

    if (g_hWndObject3 && IsWindow(g_hWndObject3))
    {
        SetWindowPos(g_hWndObject3, HWND_TOP, 0, topHeight, screenW, bottomHeight, SWP_SHOWWINDOW);
    }
}

INT_PTR CALLBACK ParamsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        SetDlgItemInt(hDlg, IDC_EDIT_NPOINT, 10, FALSE);
        SetDlgItemInt(hDlg, IDC_EDIT_XMIN, 1, TRUE);
        SetDlgItemInt(hDlg, IDC_EDIT_XMAX, 10, TRUE);
        SetDlgItemInt(hDlg, IDC_EDIT_YMIN, 1, TRUE);
        SetDlgItemInt(hDlg, IDC_EDIT_YMAX, 100, TRUE);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            int nPoint = GetDlgItemInt(hDlg, IDC_EDIT_NPOINT, NULL, FALSE);
            int xMin = GetDlgItemInt(hDlg, IDC_EDIT_XMIN, NULL, TRUE);
            int xMax = GetDlgItemInt(hDlg, IDC_EDIT_XMAX, NULL, TRUE);
            int yMin = GetDlgItemInt(hDlg, IDC_EDIT_YMIN, NULL, TRUE);
            int yMax = GetDlgItemInt(hDlg, IDC_EDIT_YMAX, NULL, TRUE);
            long params[5] = { nPoint, xMin, xMax, yMin, yMax };
            HWND hParent = GetParent(hDlg);
            if (!hParent) hParent = GetActiveWindow();
            HLOCAL mem = (HLOCAL)LocalAlloc(LPTR, sizeof(params));
            if (mem)
            {
                memcpy(mem, params, sizeof(params));
                SetPropW(hParent, L"LAB6_PARAMS", (HANDLE)mem);
                PostMessage(hParent, APP_SEND_PARAMS, 0, 0);
            }
            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB6, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!MyInitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB6));

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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB6));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB6);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL MyInitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

HWND WaitForWindow(LPCWSTR className, int timeoutMs = 5000)
{
    const int step = 100;
    int waited = 0;
    HWND h = NULL;
    while (waited < timeoutMs)
    {
        h = FindWindowW(className, NULL);
        if (h) return h;
        Sleep(step);
        waited += step;
    }
    return h;
}

void CleanupParams(HWND hWnd)
{
    HANDLE h = GetPropW(hWnd, L"LAB6_PARAMS");
    if (h) { LocalFree(h); RemovePropW(hWnd, L"LAB6_PARAMS"); }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_EXEC:
                {
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_PARAMS), hWnd, ParamsDlgProc);
                }
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
    case APP_SEND_PARAMS:
        {
            void* p = GetPropW(hWnd, L"LAB6_PARAMS");
            if (p)
            {
                long params[5];
                memcpy(params, p, sizeof(params));

                HWND hObj2 = FindWindowW(L"OBJECT2", NULL);
                if (!hObj2)
                {
                    std::wstring args = std::to_wstring((UINT_PTR)hWnd);
                    LaunchExeWithArgs(L"Object2.exe", args);
                }
                else
                {
                    SendCopyData(hObj2, hWnd, params, sizeof(params));
                }
            }
        }
        break;
    case WM_APP_OBJECT2_READY:
        {
            HWND child = (HWND)wParam;
            g_hWndObject2 = child;
            void* p = GetPropW(hWnd, L"LAB6_PARAMS");
            if (p)
            {
                long params[5]; memcpy(params, p, sizeof(params));
                SendCopyData(child, hWnd, params, sizeof(params));
            }
            ArrangeWindows(hWnd);
        }
        break;
    case WM_APP_TOO_MANY_POINTS:
        {
            int requested = (int)wParam;
            int adjusted = (int)lParam;
            wchar_t buf[256];
            _snwprintf_s(buf, _countof(buf), L"Requested %d points but range allows only %d unique X values. Generated %d points.", requested, adjusted, adjusted);
            MessageBoxW(hWnd, buf, L"Lab6: points adjusted", MB_OK | MB_ICONWARNING);
        }
        break;
    case WM_APP_DATA_GENERATED:
        {
            HWND hObj3 = FindWindowW(L"OBJECT3", NULL);
            if (!hObj3)
            {
                std::wstring args = std::to_wstring((UINT_PTR)hWnd);
                LaunchExeWithArgs(L"Object3.exe", args);
            }
            else
            {
                PostMessage(hObj3, WM_COMMAND, IDM_PROCESS_DATA, 0);
            }
        }
        break;
    case WM_APP_OBJECT3_READY:
        {
            g_hWndObject3 = (HWND)wParam;
            PostMessage(g_hWndObject3, WM_COMMAND, IDM_PROCESS_DATA, 0);
            ArrangeWindows(hWnd);
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        {
            HWND hObj2 = FindWindowW(L"OBJECT2", NULL);
            if (hObj2) PostMessage(hObj2, WM_CLOSE, 0, 0);
            HWND hObj3 = FindWindowW(L"OBJECT3", NULL);
            if (hObj3) PostMessage(hObj3, WM_CLOSE, 0, 0);
        }
        CleanupParams(hWnd);
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
