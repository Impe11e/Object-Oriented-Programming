#include "framework.h"
#include "Lab6.h"
#include <string>
#include <vector>
#include <sstream>
#include <shellapi.h>

#define MAX_LOADSTRING 100

HINSTANCE hInst;                               
WCHAR szTitle[MAX_LOADSTRING];              
WCHAR szWindowClass[MAX_LOADSTRING];         

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                MyInitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int SendCopyData(HWND hWndDest, HWND hWndSrc, void *lp, long cb)
{
    COPYDATASTRUCT cds;
    cds.dwData = 1;
    cds.cbData = cb;
    cds.lpData = lp;
    return (int)SendMessage(hWndDest, WM_COPYDATA, (WPARAM)hWndSrc, (LPARAM)&cds);
}

bool LaunchExeWithCandidates(const std::wstring &exeName)
{
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
        HINSTANCE h = ShellExecuteW(NULL, L"open", full.c_str(), NULL, NULL, SW_SHOWNORMAL);
        if ((INT_PTR)h > 32) return true;
    }

    HINSTANCE h = ShellExecuteW(NULL, L"open", exeName.c_str(), NULL, NULL, SW_SHOWNORMAL);
    if ((INT_PTR)h > 32) return true;

    return false;
}

std::wstring GetClipboardUnicodeText(HWND owner)
{
    std::wstring res;
    if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) return res;
    if (!OpenClipboard(owner)) return res;
    HGLOBAL h = GetClipboardData(CF_UNICODETEXT);
    if (h)
    {
        LPCWSTR p = (LPCWSTR)GlobalLock(h);
        if (p) res = p;
        GlobalUnlock(h);
    }
    CloseClipboard();
    return res;
}

std::vector<long> ParsePointsFromText(const std::wstring &text)
{
    std::vector<long> out;
    std::wistringstream iss(text);
    std::wstring line;
    while (std::getline(iss, line))
    {
        if (line.empty()) continue;
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        std::wistringstream ls(line);
        std::wstring a, b;
        if (std::getline(ls, a, L'\t'))
        {
            if (!std::getline(ls, b, L'\t')) {
                ls.clear(); ls.str(line);
                ls >> a >> b;
            }
            try {
                long x = std::stol(a);
                long y = std::stol(b);
                out.push_back(x);
                out.push_back(y);
            } catch(...) {
            }
        }
    }
    return out;
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
                PostMessage(hParent, WM_APP+1, 0, 0);
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
    case WM_APP+1:
        {
            void* p = GetPropW(hWnd, L"LAB6_PARAMS");
            if (p)
            {
                long params[5];
                memcpy(params, p, sizeof(params));

                HWND hObj2 = FindWindowW(L"OBJECT2", NULL);
                if (!hObj2)
                {
                    LaunchExeWithCandidates(L"Object2.exe");
                    hObj2 = WaitForWindow(L"OBJECT2", 5000);
                }
                if (hObj2)
                {
                    ShowWindow(hObj2, SW_SHOWNORMAL);
                    SetForegroundWindow(hObj2);
                    SendCopyData(hObj2, hWnd, params, sizeof(params));
                }

                HWND hObj3 = FindWindowW(L"OBJECT3", NULL);
                if (!hObj3)
                {
                    LaunchExeWithCandidates(L"Object3.exe");
                    hObj3 = WaitForWindow(L"OBJECT3", 5000);
                }
                if (hObj3)
                {
                    ShowWindow(hObj3, SW_SHOWNORMAL);
                    SetForegroundWindow(hObj3);
                }

                if (hObj2 || hObj3)
                {
                    int screenW = GetSystemMetrics(SM_CXSCREEN);
                    int screenH = GetSystemMetrics(SM_CYSCREEN);
                    int w = screenW / 2;
                    int h = screenH * 3 / 4;
                    if (hObj2) SetWindowPos(hObj2, HWND_TOP, 0, 0, w, h, SWP_SHOWWINDOW);
                    if (hObj3) SetWindowPos(hObj3, HWND_TOP, w, 0, w, h, SWP_SHOWWINDOW);
                }

                std::wstring clip;
                const int maxWait = 3000;
                int waited = 0;
                const int step = 200;
                while (waited < maxWait)
                {
                    clip = GetClipboardUnicodeText(hWnd);
                    if (!clip.empty()) break;
                    Sleep(step);
                    waited += step;
                }

                if (!clip.empty() && hObj3)
                {
                    std::vector<long> pairs = ParsePointsFromText(clip);
                    if (!pairs.empty())
                    {
                        SendCopyData(hObj3, hWnd, pairs.data(), (long)(pairs.size()*sizeof(long)));
                        PostMessage(hObj3, WM_PAINT, 0, 0);
                    }
                }

                CleanupParams(hWnd);
            }
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
