#include "toolbar.h"

Toolbar::Toolbar() : hToolbar(nullptr), hParentWnd(nullptr), hInstance(nullptr) {
}

Toolbar::~Toolbar() {
}

bool Toolbar::Create(HWND hParent, HINSTANCE hInst) {
    if (!hParent || !hInst) {
        return false;
    }

    hParentWnd = hParent;
    hInstance = hInst;

    hToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_TOP | TBSTYLE_TOOLTIPS | TBSTYLE_WRAPABLE,
        0, 0, 0, 0,
        hParentWnd, (HMENU)1000, hInstance, NULL);

    if (!hToolbar) {
        return false;
    }

    SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

    SetupImageList();

    AddButtons();

    AutoSize();

    return true;
}

void Toolbar::SetupImageList() {
    if (!hToolbar) {
        return;
    }

    HIMAGELIST hImageList = ImageList_Create(24, 24, ILC_COLOR32 | ILC_MASK, 4, 0);
    
    HBITMAP hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TOOLBAR));

    if (hBitmap) {
        ImageList_Add(hImageList, hBitmap, NULL);
        DeleteObject(hBitmap);
    }

    SendMessage(hToolbar, TB_SETIMAGELIST, 0, (LPARAM)hImageList);
}

void Toolbar::AddButtons() {
    if (!hToolbar) {
        return;
    }

    TBBUTTON tbb[4];
    ZeroMemory(tbb, sizeof(tbb));

    tbb[0].iBitmap = 0;
    tbb[0].fsState = TBSTATE_ENABLED;
    tbb[0].fsStyle = TBSTYLE_BUTTON;
    tbb[0].idCommand = ID_POINT;

    tbb[1].iBitmap = 1;
    tbb[1].fsState = TBSTATE_ENABLED;
    tbb[1].fsStyle = TBSTYLE_BUTTON;
    tbb[1].idCommand = ID_LINE;

    tbb[2].iBitmap = 2;
    tbb[2].fsState = TBSTATE_ENABLED;
    tbb[2].fsStyle = TBSTYLE_BUTTON;
    tbb[2].idCommand = ID_RECTANGLE;

    tbb[3].iBitmap = 3;
    tbb[3].fsState = TBSTATE_ENABLED;
    tbb[3].fsStyle = TBSTYLE_BUTTON;
    tbb[3].idCommand = ID_ELLIPSE;

    SendMessage(hToolbar, TB_ADDBUTTONS, 4, (LPARAM)&tbb);
}

void Toolbar::AutoSize() {
    if (hToolbar) {
        SendMessage(hToolbar, TB_AUTOSIZE, 0, 0);
    }
}
