#include "framework.h"
#include "Object2.h"
#include <string>
#include <sstream>
#include <chrono>
#include <shellapi.h>
#include <fstream>
#include <algorithm>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

#ifndef _WIN32_IE
#define _WIN32_IE 0x0500
#endif

#define MAX_LOADSTRING 100

#define WM_APP_OBJECT2_READY    (WM_USER + 1)
#define WM_APP_DATA_GENERATED   (WM_USER + 3)
#define WM_APP_TOO_MANY_POINTS  (WM_USER + 5)

class Object2App {
public:
    Object2App(): hInst_(NULL), hwnd_(NULL), hParent_(NULL), hList_(NULL) {}
    int Run(HINSTANCE hInstance, int nCmdShow)
    {
        hInst_ = hInstance;
        int argc; LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
        if (argc > 1) hParent_ = (HWND)(UINT_PTR)_wtoi(argv[1]);
        LocalFree(argv);
        LoadStringW(hInst_, IDS_APP_TITLE, szTitle_, MAX_LOADSTRING);
        LoadStringW(hInst_, IDC_OBJECT2, szWindowClass_, MAX_LOADSTRING);
        RegisterClass();
        if (!InitInstance(nCmdShow)) return FALSE;
        HACCEL hAccelTable = LoadAccelerators(hInst_, MAKEINTRESOURCE(IDC_OBJECT2));
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0))
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) { TranslateMessage(&msg); DispatchMessage(&msg); }
        }
        return (int)msg.wParam;
    }

private:
    HINSTANCE hInst_;
    HWND hwnd_;
    HWND hParent_;
    HWND hList_;
    WCHAR szTitle_[MAX_LOADSTRING];
    WCHAR szWindowClass_[MAX_LOADSTRING];
    DataGenerator generator_;

    void RegisterClass()
    {
        WNDCLASSEXW wcex; ZeroMemory(&wcex, sizeof(wcex));
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = Object2App::StaticWndProc;
        wcex.cbClsExtra = 0; wcex.cbWndExtra = 0;
        wcex.hInstance = hInst_;
        wcex.hIcon = LoadIcon(hInst_, MAKEINTRESOURCE(IDI_OBJECT2));
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
        wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_OBJECT2);
        wcex.lpszClassName = szWindowClass_;
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
        RegisterClassExW(&wcex);
    }

    BOOL InitInstance(int nCmdShow)
    {
        hwnd_ = CreateWindowW(szWindowClass_, szTitle_, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 0, 600, 400, nullptr, nullptr, hInst_, this);
        if (!hwnd_) return FALSE;
        ShowWindow(hwnd_, nCmdShow); UpdateWindow(hwnd_);
        return TRUE;
    }

    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        Object2App* pThis = nullptr;
        if (message == WM_CREATE) { CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam; pThis = (Object2App*)cs->lpCreateParams; SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)pThis); pThis->hwnd_ = hWnd; }
        else pThis = (Object2App*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
        if (pThis) return pThis->WndProc(hWnd, message, wParam, lParam);
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }

    void CreateListView()
    {
        INITCOMMONCONTROLSEX icex; icex.dwSize = sizeof(icex); icex.dwICC = ICC_LISTVIEW_CLASSES; InitCommonControlsEx(&icex);
        hList_ = CreateWindowExW(0, WC_LISTVIEWW, L"", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS,
            10,10, 560, 300, hwnd_, (HMENU)1001, hInst_, NULL);
        if (!hList_) return;
        ListView_SetExtendedListViewStyle(hList_, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        LVCOLUMNW col; ZeroMemory(&col, sizeof(col)); col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        col.pszText = const_cast<LPWSTR>(L"No"); col.cx = 50; SendMessageW(hList_, LVM_INSERTCOLUMNW, 0, (LPARAM)&col);
        col.pszText = const_cast<LPWSTR>(L"X"); col.cx = 100; SendMessageW(hList_, LVM_INSERTCOLUMNW, 1, (LPARAM)&col);
        col.pszText = const_cast<LPWSTR>(L"Y"); col.cx = 100; SendMessageW(hList_, LVM_INSERTCOLUMNW, 2, (LPARAM)&col);
    }

    void UpdateListView()
    {
        if (!hList_) return;
        ListView_DeleteAllItems(hList_);
        const auto &pts = generator_.GetPoints();
        LVITEMW lvi; ZeroMemory(&lvi, sizeof(lvi)); lvi.mask = LVIF_TEXT;
        wchar_t buf[64];
        for (int i = 0; i < (int)pts.size(); ++i)
        {
            _snwprintf_s(buf, _countof(buf), L"%d", i+1);
            lvi.iItem = i; lvi.iSubItem = 0; lvi.pszText = buf; SendMessageW(hList_, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
            _snwprintf_s(buf, _countof(buf), L"%d", pts[i].x); ListView_SetItemText(hList_, i, 1, buf);
            _snwprintf_s(buf, _countof(buf), L"%d", pts[i].y); ListView_SetItemText(hList_, i, 2, buf);
        }
    }

    LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_CREATE:
            if (hParent_) PostMessage(hParent_, WM_APP_OBJECT2_READY, (WPARAM)hWnd, 0);
            CreateListView();
            break;
        case WM_COPYDATA:
            {
                COPYDATASTRUCT *cds = (COPYDATASTRUCT*)lParam;
                if (cds && cds->cbData >= sizeof(long)*5 && cds->dwData == 1)
                {
                    long* p = (long*)cds->lpData;
                    int n = (int)p[0]; int xMin = (int)p[1]; int xMax = (int)p[2]; int yMin = (int)p[3]; int yMax = (int)p[4];
                    generator_.GeneratePoints(n, xMin, xMax, yMin, yMax);
                    generator_.PutTextToClipboardW(hWnd, [&]{ std::wostringstream ss; for (auto &pt: generator_.GetPoints()) ss<<pt.x<<L"\t"<<pt.y<<L"\r\n"; return ss.str(); }());
                    HWND hObj3 = FindWindowW(L"OBJECT3", NULL);
                    if (hObj3 && IsWindow(hObj3))
                    {
                        std::vector<long> arr; arr.reserve(generator_.GetPoints().size()*2);
                        for (auto &pt: generator_.GetPoints()) { arr.push_back((long)pt.x); arr.push_back((long)pt.y); }
                        COPYDATASTRUCT cds2; cds2.dwData = 2; cds2.cbData = (DWORD)(arr.size()*sizeof(long)); cds2.lpData = arr.data();
                        SendMessageW(hObj3, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds2);
                    }
                    if (hParent_) PostMessage(hParent_, WM_APP_DATA_GENERATED, 0, 0);
                    UpdateListView();
                }
            }
            break;
        case WM_SIZE:
            {
                RECT rc; GetClientRect(hWnd, &rc);
                if (hList_) MoveWindow(hList_, 10,10, rc.right-20, rc.bottom-40, TRUE);
            }
            break;
        case WM_PAINT:
            {
                PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps); EndPaint(hWnd, &ps);
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default: return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    }
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    Object2App app; return app.Run(hInstance, nCmdShow);
}

DataGenerator::DataGenerator() { rng.seed((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count()); }
DataGenerator::~DataGenerator() {}
void DataGenerator::GeneratePoints(int nPoints, int xMin, int xMax, int yMin, int yMax)
{
    points.clear(); if (nPoints<=0) return; if (xMax<xMin) std::swap(xMin,xMax); if (yMax<yMin) std::swap(yMin,yMax);
    long maxUnique = (long)xMax-(long)xMin+1; if (maxUnique<1) maxUnique=1; if ((long)nPoints>maxUnique) nPoints=(int)maxUnique;
    std::vector<int> xs; xs.reserve(xMax-xMin+1); for (int xx=xMin; xx<=xMax; ++xx) xs.push_back(xx);
    std::shuffle(xs.begin(), xs.end(), rng); xs.resize(nPoints); std::sort(xs.begin(), xs.end()); std::uniform_int_distribution<int> dy(yMin,yMax);
    for (int i=0;i<nPoints;++i) points.push_back({xs[i], dy(rng)});
}
bool DataGenerator::PutTextToClipboardW(HWND hWnd, const std::wstring &text)
{
    if (text.empty()) return false; if (!OpenClipboard(hWnd)) return false; EmptyClipboard(); size_t size=(text.size()+1)*sizeof(wchar_t);
    HGLOBAL hglb=GlobalAlloc(GHND,size); if (!hglb) { CloseClipboard(); return false;} void* p=GlobalLock(hglb); memcpy(p,text.c_str(),size); GlobalUnlock(hglb); SetClipboardData(CF_UNICODETEXT,hglb); CloseClipboard(); return true;
}
void DataGenerator::OnPaint(HWND hWnd, HDC hdc) { (void)hWnd; (void)hdc; }
