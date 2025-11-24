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

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
std::vector<Point> g_data;
HWND g_hParent = NULL;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
std::wstring        GetTextFromClipboardWstr(HWND hWnd);
void                ProcessData(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argc > 1) {
        g_hParent = (HWND)(UINT_PTR)_wtoi(argv[1]);
    }
    LocalFree(argv);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OBJECT3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow)) return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OBJECT3));
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

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OBJECT3));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_OBJECT3);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 500, 400, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

std::wstring GetTextFromClipboardWstr(HWND hWnd) {
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

void ProcessData(HWND hWnd) {
    g_data.clear();
    std::wstring clipboardText = GetTextFromClipboardWstr(hWnd);
    if (clipboardText.empty()) return;

    std::wistringstream ss(clipboardText);
    int x, y;
    while (ss >> x >> y) {
        g_data.push_back({ x, y });
    }

    InvalidateRect(hWnd, NULL, TRUE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        if (g_hParent) {
            PostMessage(g_hParent, WM_APP_OBJECT3_READY, (WPARAM)hWnd, 0);
        }
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_PROCESS_DATA:
            ProcessData(hWnd);
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
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        GetClientRect(hWnd, &rc);

        int margin = 50;
        MoveToEx(hdc, margin, rc.bottom - margin, NULL); LineTo(hdc, rc.right - 20, rc.bottom - margin);
        MoveToEx(hdc, margin, rc.bottom - margin, NULL); LineTo(hdc, margin, 20);
        TextOut(hdc, rc.right - 15, rc.bottom - margin - 10, L"X", 1);
        TextOut(hdc, margin - 10, 5, L"Y", 1);

        if (g_data.size() > 0) {
            int minX = 0;
            int minY = 0;
            int maxX = g_data.front().x;
            int maxY = g_data.front().y;
            for (const auto& p : g_data) {
                if (p.x < minX) minX = p.x;
                if (p.y < minY) minY = p.y;
                if (p.x > maxX) maxX = p.x;
                if (p.y > maxY) maxY = p.y;
            }
            if (maxX == minX) maxX = minX + 1;
            if (maxY == minY) maxY = minY + 1;

            double scaleX = (double)(rc.right - margin - 20 - margin) / (maxX - minX);
            double scaleY = (double)(rc.bottom - margin - 20 - margin) / (maxY - minY);

            SetBkMode(hdc, TRANSPARENT);

            const int nIntervals = 4;

            SetTextAlign(hdc, TA_CENTER | TA_TOP);
            int x_axis_y_pos = rc.bottom - margin + 5;

            for (int i = 0; i <= nIntervals; ++i)
            {
                int currentX = minX + (i * (maxX - minX) / nIntervals);
                int pixelX = margin + (int)((currentX - minX) * scaleX);

                std::wstring strX = std::to_wstring(currentX);
                TextOut(hdc, pixelX, x_axis_y_pos, strX.c_str(), (int)strX.length());
            }

            SetTextAlign(hdc, TA_RIGHT | TA_BASELINE);
            int y_axis_x_pos = margin - 10;

            for (int i = 0; i <= nIntervals; ++i)
            {
                int currentY = minY + (i * (maxY - minY) / nIntervals);
                int pixelY = rc.bottom - margin - (int)((currentY - minY) * scaleY);
                if (pixelY < 20) pixelY = 20;

                std::wstring strY = std::to_wstring(currentY);
                TextOut(hdc, y_axis_x_pos, pixelY, strY.c_str(), (int)strY.length());
            }

            std::vector<Point> sorted = g_data;
            std::sort(sorted.begin(), sorted.end(), [](const Point &a, const Point &b){ return a.x < b.x; });

            HPEN hLinePen = CreatePen(PS_SOLID, 1, RGB(100, 100, 255));
            HPEN hOldPen = (HPEN)SelectObject(hdc, hLinePen);

            int startX = margin + (int)((sorted[0].x - minX) * scaleX);
            int startY = rc.bottom - margin - (int)((sorted[0].y - minY) * scaleY);
            MoveToEx(hdc, startX, startY, NULL);

            for (size_t i = 1; i < sorted.size(); ++i) {
                int nextX = margin + (int)((sorted[i].x - minX) * scaleX);
                int nextY = rc.bottom - margin - (int)((sorted[i].y - minY) * scaleY);
                LineTo(hdc, nextX, nextY);
            }
            SelectObject(hdc, hOldPen);
            DeleteObject(hLinePen);

            HBRUSH hPointBrush = CreateSolidBrush(RGB(255, 0, 0));
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hPointBrush);
            HPEN hOld = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));

            const int pointRadius = 4;

            for (const auto& point : g_data) {
                int px = margin + (int)((point.x - minX) * scaleX);
                int py = rc.bottom - margin - (int)((point.y - minY) * scaleY);
                Ellipse(hdc, px - pointRadius, py - pointRadius, px + pointRadius, py + pointRadius);
            }

            SelectObject(hdc, hOldBrush);
            DeleteObject(hPointBrush);
            SelectObject(hdc, hOld);
        }
        EndPaint(hWnd, &ps);
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
