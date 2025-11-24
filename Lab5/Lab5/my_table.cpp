#include "my_table.h"
#include <stdio.h>
#include <string.h>
#include <windowsx.h>

HWND MyTable::hWndTable = nullptr;
HWND MyTable::hWndListView = nullptr;
HINSTANCE MyTable::hInstTable = nullptr;
std::vector<TableRow> MyTable::rows;

static MyTable::HoverCallback g_hoverCb = nullptr;
static MyTable::RemoveCallback g_removeCb = nullptr;

static WNDPROC g_oldListProc = nullptr;
static int g_lastHover = -1;
static bool g_listTracking = false;

void MyTable::Clear() {
    rows.clear();
    if (hWndListView) {
        SendMessageA(hWndListView, LVM_DELETEALLITEMS, 0, 0);
    }
}

void MyTable::SetHoverCallback(HoverCallback cb) {
    g_hoverCb = cb;
}

void MyTable::SetRemoveCallback(RemoveCallback cb) {
    g_removeCb = cb;
}

static LRESULT CALLBACK ListViewSubclass(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_MOUSEMOVE: {
        POINT pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);

        LVHITTESTINFO ht = {};
        ht.pt = pt;
        int idx = ListView_HitTest(hwnd, &ht);

        if (idx != g_lastHover) {
            if (g_lastHover != -1) {
                ListView_SetItemState(hwnd, g_lastHover, 0, LVIS_SELECTED | LVIS_FOCUSED);
            }

            g_lastHover = idx;

            if (g_lastHover != -1) {
                ListView_SetItemState(hwnd, g_lastHover, LVIS_SELECTED, LVIS_SELECTED);
            }

            if (g_hoverCb) g_hoverCb(idx);
        }

        if (!g_listTracking) {
            TRACKMOUSEEVENT tme = { sizeof(tme) };
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            g_listTracking = true;
        }
        break;
    }
    case WM_MOUSELEAVE: {
        if (g_lastHover != -1) {
            ListView_SetItemState(hwnd, g_lastHover, 0, LVIS_SELECTED | LVIS_FOCUSED);
            g_lastHover = -1;
            if (g_hoverCb) g_hoverCb(-1);
        }
        g_listTracking = false;
        break;
    }
    case WM_LBUTTONDOWN: {
        POINT pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        LVHITTESTINFO ht = {};
        ht.pt = pt;
        int idx = ListView_HitTest(hwnd, &ht);
        if (idx != -1) {
            ListView_SetItemState(hwnd, idx, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        }
        break;
    }
    }
    return CallWindowProc(g_oldListProc, hwnd, uMsg, wParam, lParam);
}

void MyTable::InitListView(HWND hListView) {
    DWORD dwStyle = GetWindowLong(hListView, GWL_STYLE);
    SetWindowLong(hListView, GWL_STYLE, dwStyle | LVS_REPORT | LVS_SHOWSELALWAYS);

    ListView_SetExtendedListViewStyle(hListView,
        LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_TRACKSELECT);
        
    LVCOLUMNA lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lvc.iSubItem = 0;
    lvc.pszText = (LPSTR)"Назва";
    lvc.cx = 150;
    SendMessageA(hListView, LVM_INSERTCOLUMNA, 0, (LPARAM)&lvc);

    lvc.iSubItem = 1;
    lvc.pszText = (LPSTR)"x1";
    lvc.cx = 70;
    SendMessageA(hListView, LVM_INSERTCOLUMNA, 1, (LPARAM)&lvc);

    lvc.iSubItem = 2;
    lvc.pszText = (LPSTR)"y1";
    lvc.cx = 70;
    SendMessageA(hListView, LVM_INSERTCOLUMNA, 2, (LPARAM)&lvc);

    lvc.iSubItem = 3;
    lvc.pszText = (LPSTR)"x2";
    lvc.cx = 70;
    SendMessageA(hListView, LVM_INSERTCOLUMNA, 3, (LPARAM)&lvc);

    lvc.iSubItem = 4;
    lvc.pszText = (LPSTR)"y2";
    lvc.cx = 70;
    SendMessageA(hListView, LVM_INSERTCOLUMNA, 4, (LPARAM)&lvc);

    g_oldListProc = (WNDPROC)SetWindowLongPtr(hListView, GWLP_WNDPROC, (LONG_PTR)ListViewSubclass);
    g_lastHover = -1;
    g_listTracking = false;
}

void MyTable::FillListView() {
    if (!hWndListView) return;

    SendMessageA(hWndListView, LVM_DELETEALLITEMS, 0, 0);

    for (size_t i = 0; i < rows.size(); ++i) {
        LVITEMA lvi;
        memset(&lvi, 0, sizeof(lvi));
        lvi.mask = LVIF_TEXT;
        lvi.iItem = (int)i;
        lvi.iSubItem = 0;
        lvi.pszText = rows[i].name;
        SendMessageA(hWndListView, LVM_INSERTITEMA, 0, (LPARAM)&lvi);

        char buffer[32];

        sprintf_s(buffer, "%ld", rows[i].x1);
        lvi.iSubItem = 1;
        lvi.pszText = buffer;
        SendMessageA(hWndListView, LVM_SETITEMTEXTA, (int)i, (LPARAM)&lvi);

        sprintf_s(buffer, "%ld", rows[i].y1);
        lvi.iSubItem = 2;
        lvi.pszText = buffer;
        SendMessageA(hWndListView, LVM_SETITEMTEXTA, (int)i, (LPARAM)&lvi);

        sprintf_s(buffer, "%ld", rows[i].x2);
        lvi.iSubItem = 3;
        lvi.pszText = buffer;
        SendMessageA(hWndListView, LVM_SETITEMTEXTA, (int)i, (LPARAM)&lvi);

        sprintf_s(buffer, "%ld", rows[i].y2);
        lvi.iSubItem = 4;
        lvi.pszText = buffer;
        SendMessageA(hWndListView, LVM_SETITEMTEXTA, (int)i, (LPARAM)&lvi);
    }
}

void MyTable::Toggle(HWND hWndParent, HINSTANCE hInst) {
    if (hWndTable) {
        Close();
        return;
    }

    hInstTable = hInst;
    hWndTable = CreateDialog(hInst, MAKEINTRESOURCE(IDD_TABLE), hWndParent, TableDlgProc);

    if (hWndTable) {
        ShowWindow(hWndTable, SW_SHOW);
    }
}

void MyTable::Add(const char* name, long x1, long y1, long x2, long y2) {
    TableRow row;
    strcpy_s(row.name, name);
    row.x1 = x1;
    row.y1 = y1;
    row.x2 = x2;
    row.y2 = y2;
    rows.push_back(row);

    if (!hWndListView) return;

    int itemCount = (int)SendMessageA(hWndListView, LVM_GETITEMCOUNT, 0, 0);

    LVITEMA lvi;
    memset(&lvi, 0, sizeof(lvi));
    lvi.mask = LVIF_TEXT;
    lvi.iItem = itemCount;
    lvi.iSubItem = 0;
    lvi.pszText = (LPSTR)name;
    SendMessageA(hWndListView, LVM_INSERTITEMA, 0, (LPARAM)&lvi);

    char buffer[32];

    sprintf_s(buffer, "%ld", x1);
    lvi.iSubItem = 1;
    lvi.pszText = buffer;
    SendMessageA(hWndListView, LVM_SETITEMTEXTA, itemCount, (LPARAM)&lvi);

    sprintf_s(buffer, "%ld", y1);
    lvi.iSubItem = 2;
    lvi.pszText = buffer;
    SendMessageA(hWndListView, LVM_SETITEMTEXTA, itemCount, (LPARAM)&lvi);

    sprintf_s(buffer, "%ld", x2);
    lvi.iSubItem = 3;
    lvi.pszText = buffer;
    SendMessageA(hWndListView, LVM_SETITEMTEXTA, itemCount, (LPARAM)&lvi);

    sprintf_s(buffer, "%ld", y2);
    lvi.iSubItem = 4;
    lvi.pszText = buffer;
    SendMessageA(hWndListView, LVM_SETITEMTEXTA, itemCount, (LPARAM)&lvi);

    SendMessageA(hWndListView, LVM_ENSUREVISIBLE, itemCount, FALSE);
}

void MyTable::Remove(int index) {
    if (index < 0 || index >= (int)rows.size()) return;
    rows.erase(rows.begin() + index);
    if (hWndListView) FillListView();
}

void MyTable::Close() {
    if (hWndTable) {
        if (g_oldListProc && hWndListView) {
            SetWindowLongPtr(hWndListView, GWLP_WNDPROC, (LONG_PTR)g_oldListProc);
            g_oldListProc = nullptr;
        }

        DestroyWindow(hWndTable);
        hWndTable = nullptr;
        hWndListView = nullptr;
    }
}

INT_PTR CALLBACK MyTable::TableDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
    {
        hWndListView = GetDlgItem(hDlg, IDC_LIST_SHAPES);
        HWND hBtnClose = GetDlgItem(hDlg, IDCANCEL);

        if (hWndListView) {
            InitListView(hWndListView);
            FillListView();
        }

        if (hBtnClose) {
            ShowWindow(hBtnClose, SW_SHOW);
            EnableWindow(hBtnClose, TRUE);
            SetWindowPos(hBtnClose, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }

        return (INT_PTR)TRUE;
    }

    case WM_SIZE:
    {
        if (hWndListView) {
            RECT rc;
            GetClientRect(hDlg, &rc);
            MoveWindow(hWndListView, 7, 7, rc.right - 14, rc.bottom - 40, TRUE);
        }
        return (INT_PTR)TRUE;
    }

    case WM_SYSCOMMAND:
        if (wParam == SC_CLOSE) {
            DestroyWindow(hDlg);
        }
        break;

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        int code = HIWORD(wParam);
        if (id == IDCANCEL && code == BN_CLICKED) {
            DestroyWindow(hDlg);
            return (INT_PTR)TRUE;
        }
        if (id == IDCANCEL && code == 0) {
            DestroyWindow(hDlg);
            return (INT_PTR)TRUE;
        }
        break;
    }

    case WM_NOTIFY:
    {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        if (!pnmh) break;
        if (pnmh->idFrom == IDC_LIST_SHAPES) {
            if (pnmh->code == LVN_ITEMCHANGED) {
                LPNMLISTVIEW p = (LPNMLISTVIEW)lParam;
                if (p->uNewState & LVIS_SELECTED) {
                    if (g_hoverCb) g_hoverCb(p->iItem);
                } else if (p->uNewState & LVIS_FOCUSED) {
                    if (g_hoverCb) g_hoverCb(p->iItem);
                }
            } else if (pnmh->code == NM_CLICK) {
                LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
                if (p->iItem != -1) {
                    if (g_removeCb) g_removeCb(p->iItem);
                    Remove(p->iItem);
                }
            } else if (pnmh->code == NM_CUSTOMDRAW) {
            }
        }
        break;
    }

    case WM_DESTROY:
        hWndTable = nullptr;
        hWndListView = nullptr;
        break;
    }
    return (INT_PTR)FALSE;
}