#include "framework.h"
#include "RGR.h"
#include "MyTable.h"
#include <commdlg.h>

#define MAX_LOADSTRING 100

HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];            
MyTableClass* g_pTable = nullptr;               

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                OnFileOpen(HWND hWnd);
void                OnFileSave(HWND hWnd);
void                OnFileSaveAs(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RGR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RGR));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RGR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_RGR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
      CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   g_pTable = new MyTableClass(hWnd, hInstance);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
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
            case IDM_FILE_OPEN:
                OnFileOpen(hWnd);
                break;
            case IDM_FILE_SAVE:
                OnFileSave(hWnd);
                break;
            case IDM_FILE_SAVEAS:
                OnFileSaveAs(hWnd);
                break;
            case IDM_EDIT_UNDO:
                if (g_pTable)
                    g_pTable->Undo();
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
    
    case WM_NOTIFY:
        if (g_pTable)
        {
            g_pTable->OnNotify(lParam);
        }
        break;
        
    case WM_SIZE:
        if (g_pTable)
        {
            g_pTable->OnSize(LOWORD(lParam), HIWORD(lParam));
        }
        break;
        
    case WM_KEYDOWN:
        if (g_pTable)
        {
            g_pTable->OnKeyDown(wParam);
        }
        break;
        
    case WM_DESTROY:
        if (g_pTable)
        {
            delete g_pTable;
            g_pTable = nullptr;
        }
        PostQuitMessage(0);
        break;
        
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void OnFileOpen(HWND hWnd)
{
    OPENFILENAMEW ofn = { 0 };
    WCHAR szFile[260] = { 0 };
    
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(WCHAR);
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileNameW(&ofn))
    {
        if (g_pTable && g_pTable->LoadFromFile(szFile))
        {
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        else
        {
            MessageBoxW(hWnd, L"Не вдалося завантажити файл!", L"Помилка", MB_OK | MB_ICONERROR);
        }
    }
}

void OnFileSave(HWND hWnd)
{
    if (!g_pTable || !g_pTable->HasData())
    {
        MessageBoxW(hWnd, L"Немає даних для збереження!", L"Увага", MB_OK | MB_ICONWARNING);
        return;
    }
    
    const wchar_t* filename = g_pTable->GetCurrentFileName();
    if (filename && wcslen(filename) > 0)
    {
        if (g_pTable->SaveToFile(filename))
        {
            MessageBoxW(hWnd, L"Файл успішно збережено!", L"Інформація", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            MessageBoxW(hWnd, L"Не вдалося зберегти файл!", L"Помилка", MB_OK | MB_ICONERROR);
        }
    }
    else
    {
        OnFileSaveAs(hWnd);
    }
}

void OnFileSaveAs(HWND hWnd)
{
    if (!g_pTable || !g_pTable->HasData())
    {
        MessageBoxW(hWnd, L"Немає даних для збереження!", L"Увага", MB_OK | MB_ICONWARNING);
        return;
    }
    
    OPENFILENAMEW ofn = { 0 };
    WCHAR szFile[260] = { 0 };
    
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(WCHAR);
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.lpstrDefExt = L"txt";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    
    if (GetSaveFileNameW(&ofn))
    {
        if (g_pTable->SaveToFileAs(szFile))
        {
            MessageBoxW(hWnd, L"Файл успішно збережено!", L"Інформація", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            MessageBoxW(hWnd, L"Не вдалося зберегти файл!", L"Помилка", MB_OK | MB_ICONERROR);
        }
    }
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
