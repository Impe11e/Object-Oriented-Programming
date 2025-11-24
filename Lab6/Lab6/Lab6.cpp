#include "framework.h"
#include "Lab6.h"
#include <string>
#include <vector>
#include <sstream>
#include <shellapi.h>

#define MAX_LOADSTRING 100

#define WM_APP_OBJECT2_READY    (WM_USER + 1)
#define WM_APP_OBJECT3_READY    (WM_USER + 2)
#define WM_APP_DATA_GENERATED   (WM_USER + 3)
#define APP_SEND_PARAMS         (WM_APP + 1)
#define IDM_PROCESS_DATA        (WM_USER + 4)
#define WM_APP_TOO_MANY_POINTS  (WM_USER + 5)

class Lab6App {
public:
    Lab6App() : hInst_(nullptr), hwnd_(nullptr), hObj2_(NULL), hObj3_(NULL) {}
    int Run(HINSTANCE hInstance, int nCmdShow)
    {
        hInst_ = hInstance;
        LoadStringW(hInst_, IDS_APP_TITLE, szTitle_, MAX_LOADSTRING);
        LoadStringW(hInst_, IDC_LAB6, szWindowClass_, MAX_LOADSTRING);
        RegisterClass();
        if (!InitInstance(nCmdShow)) return FALSE;
        HACCEL hAccelTable = LoadAccelerators(hInst_, MAKEINTRESOURCE(IDC_LAB6));
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0))
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        return (int)msg.wParam;
    }

private:
    HINSTANCE hInst_;
    HWND hwnd_;
    HWND hObj2_;
    HWND hObj3_;
    WCHAR szTitle_[MAX_LOADSTRING];
    WCHAR szWindowClass_[MAX_LOADSTRING];

    void Log(const std::wstring &s) { (void)s; }

    static int SendCopyDataStatic(HWND hWndDest, HWND hWndSrc, void *lp, long cb)
    {
        COPYDATASTRUCT cds;
        cds.dwData = 1;
        cds.cbData = cb;
        cds.lpData = lp;
        return (int)SendMessage(hWndDest, WM_COPYDATA, (WPARAM)hWndSrc, (LPARAM)&cds);
    }

    bool LaunchExeWithArgs(const std::wstring &exeName, const std::wstring &args)
    {
        auto tryCreate = [&](const std::wstring &full)->bool {
            std::wstring cmdline = L"\"" + full + L"\"" + (args.empty() ? L"" : L" " + args);
            std::vector<wchar_t> cmdbuf(cmdline.begin(), cmdline.end()); cmdbuf.push_back(0);
            STARTUPINFOW si = {0}; si.cb = sizeof(si);
            PROCESS_INFORMATION pi = {0};
            if (CreateProcessW(NULL, cmdbuf.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
            {
                CloseHandle(pi.hThread); CloseHandle(pi.hProcess); return true;
            }
            HINSTANCE h = ShellExecuteW(NULL, L"open", full.c_str(), args.empty() ? NULL : args.c_str(), NULL, SW_SHOWNORMAL);
            if ((INT_PTR)h > 32) return true;
            return false;
        };

        wchar_t found[MAX_PATH] = {0};
        DWORD len = SearchPathW(NULL, exeName.c_str(), NULL, MAX_PATH, found, NULL);
        if (len > 0 && len < MAX_PATH)
        {
            if (tryCreate(found)) return true;
        }
        bool hasExe = (exeName.size() >= 4 && _wcsicmp(exeName.c_str() + exeName.size() - 4, L".exe") == 0);
        if (!hasExe)
        {
            std::wstring withExe = exeName + L".exe";
            len = SearchPathW(NULL, withExe.c_str(), NULL, MAX_PATH, found, NULL);
            if (len > 0 && len < MAX_PATH)
            {
                if (tryCreate(found)) return true;
            }
        }

        wchar_t modulePath[MAX_PATH];
        if (GetModuleFileNameW(NULL, modulePath, MAX_PATH) == 0) {
            MessageBoxW(NULL, L"GetModuleFileNameW failed", L"Lab6 Debug", MB_OK | MB_ICONERROR);
            return false;
        }
        std::wstring path(modulePath);
        size_t pos = path.find_last_of(L"\\/");
        std::wstring dir = (pos == std::wstring::npos) ? L"." : path.substr(0, pos+1);

        std::vector<std::wstring> candidates;
        candidates.push_back(dir + exeName);
        if (!hasExe) candidates.push_back(dir + exeName + L".exe");
        candidates.push_back(dir + L"..\\Debug\\" + exeName);
        candidates.push_back(dir + L"..\\Release\\" + exeName);
        candidates.push_back(dir + L"..\\Object2\\Debug\\" + exeName);
        candidates.push_back(dir + L"..\\Object2\\Release\\" + exeName);
        candidates.push_back(dir + L"..\\Object3\\Debug\\" + exeName);
        candidates.push_back(dir + L"..\\Object3\\Release\\" + exeName);

        size_t p2 = dir.find_last_of(L"\\/", pos - 1);
        if (p2 != std::wstring::npos) {
            std::wstring parent = dir.substr(0, p2+1);
            candidates.push_back(parent + exeName);
            candidates.push_back(parent + L"Debug\\" + exeName);
            candidates.push_back(parent + L"Release\\" + exeName);
        }

        std::vector<std::pair<std::wstring,bool>> checked;
        for (auto &c : candidates)
        {
            DWORD attr = GetFileAttributesW(c.c_str());
            bool exists = (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
            checked.emplace_back(c, exists);
            if (exists)
            {
                if (tryCreate(c)) return true;
            }
        }

        if (tryCreate(exeName)) return true;
        if (!hasExe) { std::wstring withExe = exeName + L".exe"; if (tryCreate(withExe)) return true; }

        std::wostringstream dbg;
        dbg << L"LaunchExeWithArgs failed to find/run '" << exeName << L"'.\nModule dir: " << dir << L"\nTried paths:\n";
        for (auto &p : checked) dbg << (p.second ? L"[FOUND] " : L"[MISSING] ") << p.first << L"\n";
        dbg << L"Also tried SearchPath and direct CreateProcess by name.\n";
        MessageBoxW(NULL, dbg.str().c_str(), L"Lab6 Debug - LaunchExeWithArgs", MB_OK | MB_ICONERROR);

        return false;
    }

    void ArrangeWindows()
    {
        int screenW = GetSystemMetrics(SM_CXSCREEN);
        int screenH = GetSystemMetrics(SM_CYSCREEN);
        int topHeight = screenH / 3;
        int bottomHeight = screenH - topHeight;
        int leftWidth = screenW / 2;
        int rightWidth = screenW - leftWidth;
        if (hwnd_) SetWindowPos(hwnd_, HWND_TOP, 0, 0, leftWidth, topHeight, SWP_SHOWWINDOW);
        if (hObj2_ && IsWindow(hObj2_)) SetWindowPos(hObj2_, HWND_TOP, leftWidth, 0, rightWidth, topHeight, SWP_SHOWWINDOW);
        if (hObj3_ && IsWindow(hObj3_)) SetWindowPos(hObj3_, HWND_TOP, 0, topHeight, screenW, bottomHeight, SWP_SHOWWINDOW);
    }

    void RegisterClass()
    {
        WNDCLASSEXW wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = Lab6App::StaticWndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInst_;
        wcex.hIcon = LoadIcon(hInst_, MAKEINTRESOURCE(IDI_LAB6));
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
        wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB6);
        wcex.lpszClassName = szWindowClass_;
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
        RegisterClassExW(&wcex);
    }

    BOOL InitInstance(int nCmdShow)
    {
        hwnd_ = CreateWindowW(szWindowClass_, szTitle_, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInst_, this);
        if (!hwnd_) return FALSE;
        ShowWindow(hwnd_, nCmdShow);
        UpdateWindow(hwnd_);
        return TRUE;
    }

    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        Lab6App* pThis = nullptr;
        if (message == WM_CREATE) {
            CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
            pThis = (Lab6App*)cs->lpCreateParams;
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
            pThis->hwnd_ = hWnd;
        } else {
            pThis = (Lab6App*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
        }
        if (pThis) return pThis->WndProc(hWnd, message, wParam, lParam);
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }

    LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_COMMAND:
            {
                int wmId = LOWORD(wParam);
                switch (wmId)
                {
                case IDM_EXEC:
                    DialogBox(hInst_, MAKEINTRESOURCE(IDD_PARAMS), hWnd, Lab6App::StaticParamsDlgProc);
                    break;
                case IDM_ABOUT:
                    DialogBox(hInst_, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, Lab6App::StaticAbout);
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
                HANDLE h = GetPropW(hWnd, L"LAB6_PARAMS");
                if (h)
                {
                    long params[5]; memcpy(params, h, sizeof(params));
                    HWND hObj2 = FindWindowW(L"OBJECT2", NULL);

                    if (!hObj2)
                    {
                        std::wstring args = std::to_wstring((UINT_PTR)hWnd);
                        bool launched = LaunchExeWithArgs(L"Object2.exe", args);
                        if (!launched) {
                            MessageBoxW(hWnd, L"Failed to launch Object2.exe (LaunchExeWithArgs returned false)", L"Lab6 Debug", MB_OK | MB_ICONERROR);
                        }
                    }
                    else
                    {
                        SendCopyDataStatic(hObj2, hWnd, params, sizeof(params));
                    }
                }
            }
            break;
        case WM_APP_OBJECT2_READY:
            {
                HWND child = (HWND)wParam; hObj2_ = child;
                HANDLE h = GetPropW(hWnd, L"LAB6_PARAMS");
                if (h)
                {
                    long params[5]; memcpy(params, h, sizeof(params));
                    SendCopyDataStatic(child, hWnd, params, sizeof(params));
                }
                ArrangeWindows();
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
                hObj3_ = (HWND)wParam;
                PostMessage(hObj3_, WM_COMMAND, IDM_PROCESS_DATA, 0);
                ArrangeWindows();
            }
            break;
        case WM_PAINT:
            {
                PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps); EndPaint(hWnd, &ps);
            }
            break;
        case WM_DESTROY:
            {
                HWND hObj2 = FindWindowW(L"OBJECT2", NULL); if (hObj2) PostMessage(hObj2, WM_CLOSE, 0, 0);
                HWND hObj3 = FindWindowW(L"OBJECT3", NULL); if (hObj3) PostMessage(hObj3, WM_CLOSE, 0, 0);
            }
            CleanupParams(hWnd);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    }

    static INT_PTR CALLBACK StaticParamsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_INITDIALOG) {
            SetDlgItemInt(hDlg, IDC_EDIT_NPOINT, 10, FALSE);
            SetDlgItemInt(hDlg, IDC_EDIT_XMIN, 1, TRUE);
            SetDlgItemInt(hDlg, IDC_EDIT_XMAX, 10, TRUE);
            SetDlgItemInt(hDlg, IDC_EDIT_YMIN, 1, TRUE);
            SetDlgItemInt(hDlg, IDC_EDIT_YMAX, 100, TRUE);
            return (INT_PTR)TRUE;
        }
        if (message == WM_COMMAND && LOWORD(wParam) == IDOK) {
            int nPoint = GetDlgItemInt(hDlg, IDC_EDIT_NPOINT, NULL, FALSE);
            int xMin = GetDlgItemInt(hDlg, IDC_EDIT_XMIN, NULL, TRUE);
            int xMax = GetDlgItemInt(hDlg, IDC_EDIT_XMAX, NULL, TRUE);
            int yMin = GetDlgItemInt(hDlg, IDC_EDIT_YMIN, NULL, TRUE);
            int yMax = GetDlgItemInt(hDlg, IDC_EDIT_YMAX, NULL, TRUE);
            if (nPoint <= 0) { MessageBoxW(hDlg, L"nPoint must be greater than 0", L"Input error", MB_OK | MB_ICONERROR); return (INT_PTR)TRUE; }
            if (xMin > xMax) { MessageBoxW(hDlg, L"xMin cannot be greater than xMax", L"Input error", MB_OK | MB_ICONERROR); return (INT_PTR)TRUE; }
            if (yMin > yMax) { MessageBoxW(hDlg, L"yMin cannot be greater than yMax", L"Input error", MB_OK | MB_ICONERROR); return (INT_PTR)TRUE; }
            long maxUnique = (long)xMax - (long)xMin + 1; if (maxUnique < 1) maxUnique = 1;
            if ((long)nPoint > maxUnique) { std::wstring msg = L"Requested " + std::to_wstring(nPoint) + L" points but range allows only " + std::to_wstring(maxUnique) + L" unique X values.\nWill generate " + std::to_wstring(maxUnique) + L" points instead."; MessageBoxW(hDlg, msg.c_str(), L"Warning", MB_OK | MB_ICONWARNING); nPoint = (int)maxUnique; }
            long params[5] = { nPoint, xMin, xMax, yMin, yMax };
            HWND hParent = GetParent(hDlg); if (!hParent) hParent = GetActiveWindow(); HLOCAL mem = (HLOCAL)LocalAlloc(LPTR, sizeof(params));
            if (mem) { memcpy(mem, params, sizeof(params)); SetPropW(hParent, L"LAB6_PARAMS", (HANDLE)mem); PostMessage(hParent, APP_SEND_PARAMS, 0, 0); }
            EndDialog(hDlg, IDOK); return (INT_PTR)TRUE;
        }
        if (message == WM_COMMAND && LOWORD(wParam) == IDCANCEL) { EndDialog(hDlg, IDCANCEL); return (INT_PTR)TRUE; }
        return (INT_PTR)FALSE;
    }

    static INT_PTR CALLBACK StaticAbout(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
        UNREFERENCED_PARAMETER(lParam);
        switch (message)
        {
        case WM_INITDIALOG: return (INT_PTR)TRUE;
        case WM_COMMAND: if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) { EndDialog(hDlg, LOWORD(wParam)); return (INT_PTR)TRUE; } break;
        }
        return (INT_PTR)FALSE;
    }

    void CleanupParams(HWND hWnd)
    {
        HANDLE h = GetPropW(hWnd, L"LAB6_PARAMS"); if (h) { LocalFree(h); RemovePropW(hWnd, L"LAB6_PARAMS"); }
    }
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    Lab6App app;
    return app.Run(hInstance, nCmdShow);
}
