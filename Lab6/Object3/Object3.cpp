#include "framework.h"
#include "Object3.h"
#include <shellapi.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#define MAX_LOADSTRING 100

#define WM_APP_OBJECT3_READY    (WM_USER + 2)
#define IDM_PROCESS_DATA        (WM_USER + 4)

struct Point { int x, y; };

class Object3App {
public:
    Object3App() : hInst_(NULL), hwnd_(NULL), hParent_(NULL) {}

    int Run(HINSTANCE hInstance, int nCmdShow)
    {
        hInst_ = hInstance;
        int argc; LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
        if (argc > 1) { hParent_ = (HWND)(UINT_PTR)_wtoi(argv[1]); }
        LocalFree(argv);

        LoadStringW(hInst_, IDS_APP_TITLE, szTitle_, MAX_LOADSTRING);
        LoadStringW(hInst_, IDC_OBJECT3, szWindowClass_, MAX_LOADSTRING);
        RegisterClass();
        if (!InitInstance(nCmdShow)) return FALSE;

        HACCEL hAccelTable = LoadAccelerators(hInst_, MAKEINTRESOURCE(IDC_OBJECT3));
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
    HWND hParent_;
    WCHAR szTitle_[MAX_LOADSTRING];
    WCHAR szWindowClass_[MAX_LOADSTRING];
    std::vector<Point> data_;

    void RegisterClass()
    {
        WNDCLASSEXW wcex; ZeroMemory(&wcex, sizeof(wcex));
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = Object3App::StaticWndProc;
        wcex.cbClsExtra = 0; wcex.cbWndExtra = 0;
        wcex.hInstance = hInst_;
        wcex.hIcon = LoadIcon(hInst_, MAKEINTRESOURCE(IDI_OBJECT3));
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
        wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_OBJECT3);
        wcex.lpszClassName = szWindowClass_;
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
        RegisterClassExW(&wcex);
    }

    BOOL InitInstance(int nCmdShow)
    {
        hwnd_ = CreateWindowW(szWindowClass_, szTitle_, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 0, 500, 400, nullptr, nullptr, hInst_, this);
        if (!hwnd_) return FALSE;
        ShowWindow(hwnd_, nCmdShow);
        UpdateWindow(hwnd_);
        return TRUE;
    }

    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        Object3App* pThis = nullptr;
        if (message == WM_CREATE) { CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam; pThis = (Object3App*)cs->lpCreateParams; SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)pThis); pThis->hwnd_ = hWnd; }
        else pThis = (Object3App*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
        if (pThis) return pThis->WndProc(hWnd, message, wParam, lParam);
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }

    std::wstring GetTextFromClipboardWstr(HWND hWnd)
    {
        std::wstring res;
        if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) return res;
        if (!OpenClipboard(hWnd)) return res;
        HGLOBAL hData = GetClipboardData(CF_UNICODETEXT);
        if (hData == NULL) { CloseClipboard(); return L""; }
        LPCWSTR pszText = static_cast<LPCWSTR>(GlobalLock(hData));
        if (!pszText) { CloseClipboard(); return L""; }
        res = pszText;
        GlobalUnlock(hData);
        CloseClipboard();
        return res;
    }

    void ProcessDataFromClipboard(HWND hWnd)
    {
        data_.clear();
        std::wstring clipboardText = GetTextFromClipboardWstr(hWnd);
        if (clipboardText.empty()) return;
        std::wistringstream ss(clipboardText);
        int x, y;
        while (ss >> x >> y) data_.push_back({x,y});
        InvalidateRect(hWnd, NULL, TRUE);
    }

    void ProcessPointsFromCopyData(COPYDATASTRUCT* cds)
    {
        if (!cds) return;
        if (cds->dwData != 2) return;
        int count = cds->cbData / sizeof(long);
        if (count <= 0 || (count % 2) != 0) return;
        int n = count / 2;
        data_.clear();
        long* p = (long*)cds->lpData;
        for (int i = 0; i < n; ++i) data_.push_back({ (int)p[2*i], (int)p[2*i+1] });
        InvalidateRect(hwnd_, NULL, TRUE);
    }

    LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_CREATE:
            if (hParent_) PostMessage(hParent_, WM_APP_OBJECT3_READY, (WPARAM)hWnd, 0);
            break;
        case WM_COPYDATA:
            {
                COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;
                ProcessPointsFromCopyData(cds);
            }
            break;
        case WM_COMMAND:
            {
                int wmId = LOWORD(wParam);
                switch (wmId)
                {
                case IDM_PROCESS_DATA: ProcessDataFromClipboard(hWnd); break;
                case IDM_ABOUT: DialogBox(hInst_, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, Object3App::StaticAbout); break;
                case IDM_EXIT: DestroyWindow(hWnd); break;
                default: return DefWindowProc(hWnd, message, wParam, lParam);
                }
            }
            break;
        case WM_PAINT:
            {
                PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps);
                DrawGraph(hdc);
                EndPaint(hWnd, &ps);
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default: return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    }

    void DrawGraph(HDC hdc)
    {
        RECT rc; GetClientRect(hwnd_, &rc);
        FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW+1));
        int margin = 50;
        MoveToEx(hdc, margin, rc.bottom - margin, NULL); LineTo(hdc, rc.right - 20, rc.bottom - margin);
        MoveToEx(hdc, margin, rc.bottom - margin, NULL); LineTo(hdc, margin, 20);
        TextOutW(hdc, rc.right - 15, rc.bottom - margin - 10, L"X", 1);
        TextOutW(hdc, margin - 10, 5, L"Y", 1);
        if (data_.empty()) return;
        int minX = data_.front().x; int minY = data_.front().y; int maxX = minX; int maxY = minY;
        for (const auto &p : data_) { if (p.x < minX) minX = p.x; if (p.y < minY) minY = p.y; if (p.x > maxX) maxX = p.x; if (p.y > maxY) maxY = p.y; }
        if (maxX == minX) maxX = minX + 1; if (maxY == minY) maxY = minY + 1;
        double scaleX = (double)(rc.right - margin - 20 - margin) / (maxX - minX);
        double scaleY = (double)(rc.bottom - margin - 20 - margin) / (maxY - minY);
        SetBkMode(hdc, TRANSPARENT);
        const int nIntervals = 4;
        SetTextAlign(hdc, TA_CENTER | TA_TOP);
        int x_axis_y_pos = rc.bottom - margin + 5;
        for (int i = 0; i <= nIntervals; ++i) { int currentX = minX + (i * (maxX - minX) / nIntervals); int pixelX = margin + (int)((currentX - minX) * scaleX); std::wstring strX = std::to_wstring(currentX); TextOutW(hdc, pixelX, x_axis_y_pos, strX.c_str(), (int)strX.length()); }
        SetTextAlign(hdc, TA_RIGHT | TA_BASELINE);
        int y_axis_x_pos = margin - 10;
        for (int i = 0; i <= nIntervals; ++i) { int currentY = minY + (i * (maxY - minY) / nIntervals); int pixelY = rc.bottom - margin - (int)((currentY - minY) * scaleY); if (pixelY < 20) pixelY = 20; std::wstring strY = std::to_wstring(currentY); TextOutW(hdc, y_axis_x_pos, pixelY, strY.c_str(), (int)strY.length()); }
        std::vector<Point> sorted = data_; std::sort(sorted.begin(), sorted.end(), [](const Point &a, const Point &b){ return a.x < b.x; });
        HPEN hLinePen = CreatePen(PS_SOLID, 1, RGB(100,100,255)); HPEN hOldPen = (HPEN)SelectObject(hdc, hLinePen);
        int startX = margin + (int)((sorted[0].x - minX) * scaleX); int startY = rc.bottom - margin - (int)((sorted[0].y - minY) * scaleY); MoveToEx(hdc, startX, startY, NULL);
        for (size_t i=1;i<sorted.size();++i) { int nextX = margin + (int)((sorted[i].x - minX) * scaleX); int nextY = rc.bottom - margin - (int)((sorted[i].y - minY) * scaleY); LineTo(hdc, nextX, nextY); }
        SelectObject(hdc, hOldPen); DeleteObject(hLinePen);
        HBRUSH hPointBrush = CreateSolidBrush(RGB(255,0,0)); HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hPointBrush); HPEN hOld = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));
        const int pointRadius = 4; for (const auto &pt : data_) { int px = margin + (int)((pt.x - minX) * scaleX); int py = rc.bottom - margin - (int)((pt.y - minY) * scaleY); Ellipse(hdc, px-pointRadius, py-pointRadius, px+pointRadius, py+pointRadius); }
        SelectObject(hdc, hOldBrush); DeleteObject(hPointBrush); SelectObject(hdc, hOld);
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
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    Object3App app; return app.Run(hInstance, nCmdShow);
}
