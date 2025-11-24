#include "framework.h"
#include "Lab5.h"
#include "my_editor.h"
#include "my_table.h"
#include "toolbar.h"
#include "point.h"
#include "line.h"
#include "rectangle.h"
#include "ellipse.h"
#include "linewithcircles.h"
#include "cube.h"
#include <commctrl.h>
#include <functional>
#include <commdlg.h>
#pragma comment(lib, "Comctl32.lib")

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

Toolbar toolbar;

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

    InitCommonControls();

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB5, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB5));
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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB5));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB5);
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

    if (!toolbar.Create(hWnd, hInstance))
    {
        // logger?
    }

    MyEditor* ped = MyEditor::getInstance();
    if (ped) ped->AttachMainWindow(hWnd);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    MyEditor* ped = MyEditor::getInstance();

    switch (message)
    {
    case WM_SIZE:
        if (ped) ped->OnSize(hWnd, toolbar.GetHandle());
        break;

    case WM_INITMENUPOPUP:
        if (ped) ped->OnInitMenuPopup(hWnd, wParam);
        break;

    case WM_NOTIFY:
        if (ped) ped->OnNotify(hWnd, wParam, lParam);
        break;

    case WM_LBUTTONDOWN:
        if (ped) ped->OnLBdown(hWnd);
        break;

    case WM_LBUTTONUP:
        if (ped) ped->OnLBup(hWnd);
        break;

    case WM_MOUSEMOVE:
        if (ped) ped->OnMouseMove(hWnd);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        if (ped) {
            ped->OnPaint(hWnd, hdc);
        }
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        static const std::pair<int, std::function<Shape* ()>> shapeCommands[] = {
            { ID_POINT,   []() -> Shape* { return new PointShape(); } },
            { ID_LINE,    []() -> Shape* { return new LineShape(); } },
            { ID_RECTANGLE,[]() -> Shape* { return new RectShape(); } },
            { ID_ELLIPSE, []() -> Shape* { return new EllipseShape(); } },
            { ID_LINEOO,  []() -> Shape* { return new LineOOShape(); } },
            { ID_CUBE,    []() -> Shape* { return new CubeShape(); } }
        };

        bool handled = false;
        for (const auto& entry : shapeCommands) {
            if (wmId == entry.first) {
                ped->AttachToolbar(toolbar.GetHandle());
                ped->Start(entry.second());
                ped->OnToolButtonClick(wParam);
                ped->UpdateWindowTitle(hWnd, szTitle);
                handled = true;
                break;
            }
        }
        if (handled) break;

        switch (wmId)
        {
        case IDM_LOAD: {
            OPENFILENAMEW ofn = { 0 };
            wchar_t szFileW[MAX_PATH];
            szFileW[0] = L'\0';
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0";
            ofn.lpstrFile = szFileW;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
            ofn.nFilterIndex = 1;
            if (GetOpenFileNameW(&ofn)) {
                int res = MessageBoxW(hWnd, L"Replace current objects?\nYes = replace, No = append", L"Load", MB_YESNOCANCEL | MB_ICONQUESTION);
                if (res == IDYES || res == IDNO) {
                    char ansiPath[MAX_PATH];
                    int conv = WideCharToMultiByte(CP_UTF8, 0, szFileW, -1, ansiPath, MAX_PATH, NULL, NULL);
                    if (conv > 0) {
                        ped->LoadFromFile(ansiPath, res == IDYES);
                    }
                }
            }
            break;
        }
        case IDM_SAVEAS: {
            OPENFILENAMEW ofn = { 0 };
            wchar_t szFileW[MAX_PATH];
            szFileW[0] = L'\0';
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0";
            ofn.lpstrFile = szFileW;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
            ofn.nFilterIndex = 1;
            if (GetSaveFileNameW(&ofn)) {
                char ansiPath[MAX_PATH];
                int conv = WideCharToMultiByte(CP_UTF8, 0, szFileW, -1, ansiPath, MAX_PATH, NULL, NULL);
                if (conv > 0) {
                    ped->SaveToFile(ansiPath);
                }
            }
            break;
        }

        case IDM_TABLE:
            MyTable::Toggle(hWnd, hInst);
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
        MyTable::Close();
        MyEditor::Cleanup();
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
