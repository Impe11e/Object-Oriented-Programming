﻿#include "framework.h"
#include "Lab4.h"
#include "my_editor.h"
#include "toolbar.h"
#include <commctrl.h>
#pragma comment(lib, "Comctl32.lib")

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

MyEditor* ped = nullptr;
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
    LoadStringW(hInstance, IDC_LAB4, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB4));
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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB4));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB4);
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
    else {
        if (ped) ped->AttachToolbar(toolbar.GetHandle());
    }

    if (ped) ped->UpdateWindowTitle(hWnd, szTitle);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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
        switch (wmId)
        {
        case ID_POINT:
            if (!ped) ped = new MyEditor();
            ped->Start(new PointShape());
            ped->OnToolButtonClick(wParam);
            ped->UpdateWindowTitle(hWnd, szTitle);
            break;

        case ID_LINE:
            if (!ped) ped = new MyEditor();
            ped->Start(new LineShape());
            ped->OnToolButtonClick(wParam);
            ped->UpdateWindowTitle(hWnd, szTitle);
            break;

        case ID_RECTANGLE:
            if (!ped) ped = new MyEditor();
            ped->Start(new RectShape());
            ped->OnToolButtonClick(wParam);
            ped->UpdateWindowTitle(hWnd, szTitle);
            break;

        case ID_ELLIPSE:
            if (!ped) ped = new MyEditor();
            ped->Start(new EllipseShape());
            ped->OnToolButtonClick(wParam);
            ped->UpdateWindowTitle(hWnd, szTitle);
            break;

        case ID_LINEOO:
            if (!ped) ped = new MyEditor();
            ped->Start(new LineOOShape());
            ped->OnToolButtonClick(wParam);
            ped->UpdateWindowTitle(hWnd, szTitle);
            break;

        case ID_CUBE:
            if (!ped) ped = new MyEditor();
            ped->Start(new CubeShape());
            ped->OnToolButtonClick(wParam);
            ped->UpdateWindowTitle(hWnd, szTitle);
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
        delete ped;
        ped = nullptr;
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
