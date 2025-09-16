#include "framework.h"
#include "resource2.h"

static INT_PTR CALLBACK Work2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static void onClickOK(HWND hDlg);
static void UpdateStaticText(HWND hDlg, int value);

static int *globalSelectedValue = nullptr;

int Func_MOD2(HWND hWnd, HINSTANCE hI, int *selectedValue)
{
    globalSelectedValue = selectedValue;
    return DialogBox(hI, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, Work2);
}

static INT_PTR CALLBACK Work2(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    switch (iMessage)
    {
    case WM_INITDIALOG:
        {
            HWND hWndScroll = GetDlgItem(hDlg, IDC_SCROLLBAR1);
            SetScrollRange(hWndScroll, SB_CTL, 1, 100, FALSE);
            SetScrollPos(hWndScroll, SB_CTL, 1, TRUE);
            UpdateStaticText(hDlg, 1);
            
            return (INT_PTR)TRUE;
        }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) onClickOK(hDlg);
        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, 0);
        }
        break;

    case WM_HSCROLL:
        {
            HWND hWndScroll = GetDlgItem(hDlg, IDC_SCROLLBAR1);
            int pos = GetScrollPos(hWndScroll, SB_CTL);
            
            switch (LOWORD(wParam))
            {
            case SB_LINELEFT:
                pos--;
                break;
            case SB_LINERIGHT:
                pos++;
                break;
            case SB_THUMBPOSITION:
            case SB_THUMBTRACK:
                pos = HIWORD(wParam);
                break;
            default: 
                break;
            }
            
            SetScrollPos(hWndScroll, SB_CTL, pos, TRUE);
            UpdateStaticText(hDlg, pos);
            break;
        }
    case WM_CLOSE:
        EndDialog(hDlg, 0);
        break;

    default: 
        break;
    }
    return (INT_PTR)FALSE;
}

static void UpdateStaticText(HWND hDlg, int value)
{
    SetDlgItemInt(hDlg, IDC_STATIC1, value, FALSE);
}

void onClickOK(HWND hDlg) {
    if (globalSelectedValue != nullptr) {
        HWND hWndScroll = GetDlgItem(hDlg, IDC_SCROLLBAR1);
        int currentPos = GetScrollPos(hWndScroll, SB_CTL);
        *globalSelectedValue = currentPos;
    }
    EndDialog(hDlg, 1);
}
