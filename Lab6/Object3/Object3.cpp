#include "framework.h"
#include "Object3.h"
#include <vector>
#include <cwchar>

#define MAX_LOADSTRING 100

HINSTANCE hInst;                           
WCHAR szTitle[MAX_LOADSTRING];              
WCHAR szWindowClass[MAX_LOADSTRING];         

struct Pt { int x; int y; };
static std::vector<Pt> g_points;

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

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OBJECT3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OBJECT3));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_OBJECT3);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

void DrawGraph(HWND hWnd, HDC hdc)
{
    RECT rc; GetClientRect(hWnd, &rc);
    FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW+1));
    if (g_points.empty()) return;

    int margin = 40;
    int w = rc.right - rc.left - margin*2;
    int h = rc.bottom - rc.top - margin*2;
    if (w<=0 || h<=0) return;

    int xMin = g_points.front().x, xMax = g_points.front().x;
    int yMin = g_points.front().y, yMax = g_points.front().y;
    for (size_t ii = 0; ii < g_points.size(); ++ii) { auto &p = g_points[ii]; if (p.x < xMin) xMin = p.x; if (p.x> xMax) xMax = p.x; if (p.y<yMin) yMin=p.y; if (p.y>yMax) yMax=p.y; }
    int rangeX = xMax - xMin; if (rangeX==0) rangeX=1;
    int rangeY = yMax - yMin; if (rangeY==0) rangeY=1;

    HPEN hOldPen = (HPEN)SelectObject(hdc, GetStockObject(BLACK_PEN));
    LOGBRUSH lb = { BS_SOLID, RGB(0,0,0), 0 };
    HPEN hAxisPen = CreatePen(PS_SOLID, 2, RGB(0,0,0));
    SelectObject(hdc, hAxisPen);
    MoveToEx(hdc, margin, margin + h, NULL); LineTo(hdc, margin + w, margin + h);
    MoveToEx(hdc, margin, margin + h, NULL); LineTo(hdc, margin, margin);
    SelectObject(hdc, hOldPen);
    DeleteObject(hAxisPen);

    std::vector<POINT> pts;
    pts.reserve(g_points.size());
    for (size_t i=0;i<g_points.size();++i)
    {
        int sx = margin + (int)((long long)(g_points[i].x - xMin) * w / rangeX);
        int sy = margin + h - (int)((long long)(g_points[i].y - yMin) * h / rangeY);
        POINT pt = { sx, sy };
        pts.push_back(pt);
    }

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(100,100,100));
    HPEN hOld = (HPEN)SelectObject(hdc, hPen);
    if (!pts.empty()) Polyline(hdc, pts.data(), (int)pts.size());
    SelectObject(hdc, hOld);
    DeleteObject(hPen);

    HBRUSH hBrushDot = CreateSolidBrush(RGB(0,0,0));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrushDot);
    for (size_t i=0;i<pts.size();++i)
    {
        int r = 4;
        Ellipse(hdc, pts[i].x - r, pts[i].y - r, pts[i].x + r, pts[i].y + r);
    }
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrushDot);

    wchar_t buf[64];
    _snwprintf_s(buf, _countof(buf), L"%d", xMin);
    TextOutW(hdc, margin, margin + h + 2, buf, (int)wcslen(buf));
    _snwprintf_s(buf, _countof(buf), L"%d", xMax);
    TextOutW(hdc, margin + w - 30, margin + h + 2, buf, (int)wcslen(buf));
    _snwprintf_s(buf, _countof(buf), L"%d", yMin);
    TextOutW(hdc, 2, margin + h - 10, buf, (int)wcslen(buf));
    _snwprintf_s(buf, _countof(buf), L"%d", yMax);
    TextOutW(hdc, 2, margin - 10, buf, (int)wcslen(buf));
}

void OnCopyDataHandle(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    COPYDATASTRUCT *cds = (COPYDATASTRUCT*)lParam;
    if (!cds) return;
    if (cds->cbData >= sizeof(long)*5 && cds->cbData <= sizeof(long)*5)
    {
    }
    else
    {
        int count = cds->cbData / sizeof(long);
        if (count % 2 == 0)
        {
            int n = count/2;
            g_points.clear();
            long *p = (long*)cds->lpData;
            for (int i=0;i<n;i++) g_points.push_back({(int)p[2*i], (int)p[2*i+1]});
            InvalidateRect(hWnd, NULL, TRUE);
        }
    }
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 600, 400, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COPYDATA:
        OnCopyDataHandle(hWnd, wParam, lParam);
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            DrawGraph(hWnd, hdc);
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
