#include "framework.h"
#include "Object2.h"
#include <string>
#include <sstream>
#include <chrono>
#include <shellapi.h>
#include <fstream>
#include <algorithm>

#define MAX_LOADSTRING 100

#define WM_APP_OBJECT2_READY    (WM_USER + 1)
#define WM_APP_DATA_GENERATED   (WM_USER + 3)
#define WM_APP_TOO_MANY_POINTS  (WM_USER + 5)

HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];            
HWND g_hParent = NULL;
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

static void AppendLog(const std::wstring &s)
{
    wchar_t modulePath[MAX_PATH];
    if (GetModuleFileNameW(NULL, modulePath, MAX_PATH) == 0) return;
    std::wstring path(modulePath);
    size_t pos = path.find_last_of(L"\\/");
    std::wstring dir = (pos == std::wstring::npos) ? L"." : path.substr(0, pos+1);
    std::wstring log = dir + L"lab_debug.txt";
    std::wofstream ofs(log, std::ios::app);
    if (ofs) ofs << s << std::endl;
}

DataGenerator::DataGenerator()
{
    rng.seed((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
}
DataGenerator::~DataGenerator() {}

static DataGenerator g_generator;

void DataGenerator::GeneratePoints(int nPoints, int xMin, int xMax, int yMin, int yMax)
{
    points.clear();
    if (nPoints <= 0) return;
    if (xMax < xMin) std::swap(xMin, xMax);
    if (yMax < yMin) std::swap(yMin, yMax);

    long maxUniqueX = (long)xMax - (long)xMin + 1;
    if (maxUniqueX < 1) maxUniqueX = 1;

    if ((long)nPoints > maxUniqueX)
    {
        nPoints = (int)maxUniqueX;
    }

    points.clear();
    if (nPoints > 0)
    {
        std::vector<int> xs;
        xs.reserve(xMax - xMin + 1);
        for (int xx = xMin; xx <= xMax; ++xx) xs.push_back(xx);
        std::shuffle(xs.begin(), xs.end(), rng);
        xs.resize(nPoints);
        std::sort(xs.begin(), xs.end());

        std::uniform_int_distribution<int> dy(yMin, yMax);
        for (int i = 0; i < nPoints; ++i)
        {
            int x = xs[i];
            int y = dy(rng);
            points.push_back({x, y});
        }
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
    AppendLog(L"OnCopyData called");
    COPYDATASTRUCT *cds = (COPYDATASTRUCT*)lParam;
    if (!cds) return;
    if (cds->cbData >= sizeof(long)*5)
    {
        long* p = (long*)cds->lpData;
        int origNPoints = (int)p[0];
        int nPoints = origNPoints;
        int xMin = (int)p[1];
        int xMax = (int)p[2];
        int yMin = (int)p[3];
        int yMax = (int)p[4];
        std::wostringstream sslog; sslog << L"Params received: n=" << origNPoints << L" xMin=" << xMin << L" xMax=" << xMax << L" yMin=" << yMin << L" yMax=" << yMax; AppendLog(sslog.str());

        long maxUniqueX = (long)xMax - (long)xMin + 1;
        if (maxUniqueX < 1) maxUniqueX = 1;
        bool reduced = false;
        if ((long)nPoints > maxUniqueX) { nPoints = (int)maxUniqueX; reduced = true; }

        points.clear();
        if (nPoints > 0)
        {
            std::vector<int> xs;
            xs.reserve(xMax - xMin + 1);
            for (int xx = xMin; xx <= xMax; ++xx) xs.push_back(xx);
            std::shuffle(xs.begin(), xs.end(), rng);
            xs.resize(nPoints);
            std::sort(xs.begin(), xs.end());

            std::uniform_int_distribution<int> dy(yMin, yMax);
            for (int i = 0; i < nPoints; ++i)
            {
                points.push_back({ xs[i], dy(rng) });
            }
        }

        std::wostringstream ss;
        for (auto &pt : points)
        {
            ss << pt.x << L"\t" << pt.y << L"\r\n";
        }
        PutTextToClipboardW(hWnd, ss.str());
        if (reduced && g_hParent)
        {
            PostMessage(g_hParent, WM_APP_TOO_MANY_POINTS, (WPARAM)origNPoints, (LPARAM)nPoints);
            AppendLog(L"Posted WM_APP_TOO_MANY_POINTS");
        }
        if (g_hParent) {
            PostMessage(g_hParent, WM_APP_DATA_GENERATED, 0, 0);
            AppendLog(L"Posted WM_APP_DATA_GENERATED to parent");
        }

        InvalidateRect(hWnd, NULL, TRUE);
    }
}

void DataGenerator::OnPaint(HWND hWnd, HDC hdc)
{
    RECT rc;
    GetClientRect(hWnd, &rc);
    FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW+1));

    TEXTMETRIC tm; GetTextMetrics(hdc, &tm);
    int lineHeight = tm.tmHeight + tm.tmExternalLeading + 4;

    int topMargin = 10;
    int y = topMargin;
    int idx = 1;
    for (auto &pt : points)
    {
        std::wostringstream ss;
        ss << idx << L": (" << pt.x << L", " << pt.y << L")";
        std::wstring line = ss.str();
        TextOutW(hdc, 10, y, line.c_str(), (int)line.size());
        y += lineHeight;
        idx++;
    }
}

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

    std::wostringstream s; s << L"Object2 start, parentHWND=" << (UINT_PTR)g_hParent; AppendLog(s.str());

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        if (g_hParent) {
            PostMessage(g_hParent, WM_APP_OBJECT2_READY, (WPARAM)hWnd, 0);
            AppendLog(L"Posted WM_APP_OBJECT2_READY to parent");
        }
        break;
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
