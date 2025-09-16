#include "framework.h"
#include "resource1.h"

static INT_PTR CALLBACK Work1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static void onClickOK(HWND hDlg);
static WCHAR* tmp;

int Func_MOD1(HWND hWnd, HINSTANCE hI, WCHAR* dest)
{   
    tmp = dest;
	return DialogBox(hI, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Work1);
}

static INT_PTR CALLBACK Work1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) onClickOK(hDlg);
        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, 0);
        }
        break;

    case WM_CLOSE:
        EndDialog(hDlg, 0);
        break;
    }
    return (INT_PTR)FALSE;
}

void onClickOK(HWND hDlg) {
    GetDlgItemText(hDlg, IDC_EDIT1, tmp, 256);
    EndDialog(hDlg, 1);
}