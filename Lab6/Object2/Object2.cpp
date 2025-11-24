#include "framework.h"
#include "Object2.h"
#include <string>
#include <sstream>
#include <chrono>

#define MAX_LOADSTRING 100

HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];            

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

DataGenerator::DataGenerator()
{
    rng.seed((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
}
DataGenerator::~DataGenerator() {}

void DataGenerator::GeneratePoints(int nPoints, int xMin, int xMax, int yMin, int yMax)
{
    points.clear();
    if (nPoints <= 0) return;
    if (xMax < xMin) std::swap(xMin, xMax);
    if (yMax < yMin) std::swap(yMin, yMax);

    std::uniform_int_distribution<int> dx(xMin, xMax);
    std::uniform_int_distribution<int> dy(yMin, yMax);

    std::vector<int> xs;
    xs.reserve(nPoints);
    for (int i = 0; i < nPoints; ++i) xs.push_back(dx(rng));
    std::sort(xs.begin(), xs.end());

    for (int i = 0; i < nPoints; ++i)
    {
        int x = xs[i];
        int y = dy(rng);
        points.push_back({x, y});
    }
}

bool DataGenerator::PutTextToClipboardW(HWND hWnd, const std::wstring &text)
{
    if (text.empty()) return false;
    if (!OpenClipboard(hWnd)) return false;
    EmptyClipboard();
    size_t size = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL hglb = GlobalAlloc(GHND, size);
    if (!hglb) { CloseClipboard(); return false; }
    void* p = GlobalLock(hglb);
    memcpy(p, text.c_str(), size);
    GlobalUnlock(hglb);
    SetClipboardData(CF_UNICODETEXT, hglb);
    CloseClipboard();
    return true;
}

void DataGenerator::OnCopyData(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    COPYDATASTRUCT *cds = (COPYDATASTRUCT*)lParam;
    if (!cds) return;
    if (cds->cbData >= sizeof(long)*5)
    {
        long* p = (long*)cds->lpData;
        int nPoints = (int)p[0];
        int xMin = (int)p[1];
        int xMax = (int)p[2];
        int yMin = (int)p[3];
        int yMax = (int)p[4];
        GeneratePoints(nPoints, xMin, xMax, yMin, yMax);

        std::wostringstream ss;
        for (auto &pt : points)
        {
            ss << pt.x << L"\t" << pt.y << L"\r\n";
        }
        PutTextToClipboardW(hWnd, ss.str());
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

void DataGenerator::OnPaint(HWND hWnd, HDC hdc)
{
    RECT rc;
    GetClientRect(hWnd, &rc);
    FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW+1));

    int y = 10;
    for (auto &pt : points)
    {
        std::wostringstream ss;
        ss << L"(" << pt.x << L", " << pt.y << L")";
        std::wstring line = ss.str();
        TextOutW(hdc, 10, y, line.c_str(), (int)line.size());
        y += 20;
    }
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OBJECT2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OBJECT2));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OBJECT2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_OBJECT2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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

static DataGenerator g_generator;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
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
    case WM_COPYDATA:
        {
            g_generator.OnCopyData(hWnd, wParam, lParam);
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            g_generator.OnPaint(hWnd, hdc);
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
