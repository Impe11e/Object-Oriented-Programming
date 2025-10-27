#include "my_editor.h"
#include "Lab4.h"
#include <commctrl.h>
#include <wchar.h>

ShapeObjectsEditor::ShapeObjectsEditor() : currentEditor(nullptr), currentType(NO_EDITOR) {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        pcshape[i] = nullptr;
    }
    shapeCount = 0;
}

ShapeObjectsEditor::~ShapeObjectsEditor() {
    for (int i = 0; i < shapeCount; i++) {
        delete pcshape[i];
    }
    delete currentEditor;
}

void PointEditor::OnLBdown(HWND hWnd) {
    if (CheckArrayOverflow(hWnd)) return;

    POINT pt = GetMousePos(hWnd);
    pcshape[*shapeCount] = new PointShape();
    pcshape[*shapeCount]->Set(pt.x, pt.y, pt.x, pt.y);
    (*shapeCount)++;
    Invalidate(hWnd);
}

template<typename T>
Editor* CreateEditor(Shape** shapes, int* count, int size) {
    return new T(shapes, count, size);
}

struct EditorFactory {
    EditorType type;
    Editor* (*create)(Shape**, int*, int);
};

static const EditorFactory factories[] = {
    {POINT_EDITOR, CreateEditor<PointEditor>},
    {LINE_EDITOR, CreateEditor<LineEditor>},
    {RECT_EDITOR, CreateEditor<RectEditor>},
    {ELLIPSE_EDITOR, CreateEditor<EllipseEditor>},
    {LINEOO_EDITOR, CreateEditor<LineOOEditor>},
    {CUBE_EDITOR, CreateEditor<CubeEditor>}
};

void ShapeObjectsEditor::StartEditor(EditorType type) {
    for (const auto& f : factories) {
        if (f.type == type) {
            delete currentEditor;
            currentEditor = f.create(pcshape, &shapeCount, ARRAY_SIZE);
            currentType = type;
            return;
        }
    }
}

void ShapeObjectsEditor::StartPointEditor(HWND hWnd) {
    StartEditor(POINT_EDITOR);
}

void ShapeObjectsEditor::StartLineEditor(HWND hWnd) {
    StartEditor(LINE_EDITOR);
}

void ShapeObjectsEditor::StartRectEditor(HWND hWnd) {
    StartEditor(RECT_EDITOR);
}

void ShapeObjectsEditor::StartEllipseEditor(HWND hWnd) {
    StartEditor(ELLIPSE_EDITOR);
}

void ShapeObjectsEditor::StartLineOOEditor(HWND hWnd) {
    StartEditor(LINEOO_EDITOR);
}

void ShapeObjectsEditor::StartCubeEditor(HWND hWnd) {
    StartEditor(CUBE_EDITOR);
}

const WCHAR* ShapeObjectsEditor::GetCurrentShapeName() const {
    static const WCHAR* names[] = {
        L"", 
        L"Крапка - ", 
        L"Лінія - ", 
        L"Прямокутник - ", 
        L"Еліпс - ",
        L"Лінія з кружечками - ",
        L"Куб - "
    };
    return names[currentType];
}

void ShapeObjectsEditor::OnLBdown(HWND hWnd) {
    if (currentEditor)
        currentEditor->OnLBdown(hWnd);
}

void ShapeObjectsEditor::OnLBup(HWND hWnd) {
    if (currentEditor) {
        currentEditor->OnLBup(hWnd);
        InvalidateRect(hWnd, NULL, FALSE);
    }
}

void ShapeObjectsEditor::OnMouseMove(HWND hWnd) {
    if (currentEditor)
        currentEditor->OnMouseMove(hWnd);
}

void ShapeObjectsEditor::OnPaint(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
    
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
    
    HBRUSH hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    FillRect(hdcMem, &rcClient, hbrBkGnd);
    DeleteObject(hbrBkGnd);
    
    HPEN hBlackPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdcMem, hBlackPen);

    for (int i = 0; i < shapeCount; i++) {
        if (pcshape[i]) {
            pcshape[i]->Show(hdcMem);
        }
    }

    SelectObject(hdcMem, hOldPen);
    DeleteObject(hBlackPen);
    
    if (currentEditor) {
        currentEditor->OnPaint(hdcMem);
    }
    
    BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, hdcMem, 0, 0, SRCCOPY);
    
    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    
    EndPaint(hWnd, &ps);
}

void ShapeObjectsEditor::OnInitMenuPopup(HWND hWnd, WPARAM wParam) {
    HMENU hMenu = (HMENU)wParam;
    
    struct MenuItemData {
        int id;
        EditorType type;
    };
    
    static const MenuItemData menuItems[] = {
        {ID_POINT, POINT_EDITOR},
        {ID_LINE, LINE_EDITOR},
        {ID_RECTANGLE, RECT_EDITOR},
        {ID_ELLIPSE, ELLIPSE_EDITOR},
        {ID_LINEOO, LINEOO_EDITOR},
        {ID_CUBE, CUBE_EDITOR}
    };
    
    for (const auto& item : menuItems) {
        CheckMenuItem(hMenu, item.id, 
            currentType == item.type ? MF_CHECKED : MF_UNCHECKED);
    }
}

void ShapeObjectsEditor::OnToolButton(HWND hWnd, HWND hwndToolBar, int toolID) {
    struct ButtonHandler {
        int id;
        void (ShapeObjectsEditor::*handler)(HWND);
    };
    
    static const ButtonHandler handlers[] = {
        {ID_POINT, &ShapeObjectsEditor::StartPointEditor},
        {ID_LINE, &ShapeObjectsEditor::StartLineEditor},
        {ID_RECTANGLE, &ShapeObjectsEditor::StartRectEditor},
        {ID_ELLIPSE, &ShapeObjectsEditor::StartEllipseEditor},
        {ID_LINEOO, &ShapeObjectsEditor::StartLineOOEditor},
        {ID_CUBE, &ShapeObjectsEditor::StartCubeEditor}
    };
    
    for (const auto& handler : handlers) {
        if (toolID == handler.id) {
            (this->*handler.handler)(hWnd);
            break;
        }
    }
    
    static const int buttonIDs[] = {ID_POINT, ID_LINE, ID_RECTANGLE, ID_ELLIPSE, ID_LINEOO, ID_CUBE};
    for (int id : buttonIDs) {
        SendMessage(hwndToolBar, TB_PRESSBUTTON, id, MAKELONG(FALSE, 0));
    }
    
    SendMessage(hwndToolBar, TB_PRESSBUTTON, toolID, MAKELONG(TRUE, 0));
}

void ShapeObjectsEditor::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    LPNMHDR pnmh = (LPNMHDR)lParam;
    if (pnmh->code != TTN_NEEDTEXT) return;
    
    LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
    
    struct TooltipData {
        int id;
        const WCHAR* text;
    };
    
    static const TooltipData tooltips[] = {
        {ID_POINT, L"Крапка"},
        {ID_LINE, L"Лінія"},
        {ID_RECTANGLE, L"Прямокутник"},
        {ID_ELLIPSE, L"Еліпс"},
        {ID_LINEOO, L"Лінія з кружечками"},
        {ID_CUBE, L"Куб"}
    };
    
    for (const auto& tip : tooltips) {
        if (lpttt->hdr.idFrom == tip.id) {
            lstrcpy(lpttt->szText, tip.text);
            break;
        }
    }
}

void ShapeObjectsEditor::UpdateWindowTitle(HWND hWnd, LPCWSTR szBaseTitle) {
    WCHAR title[256];
    wcscpy_s(title, _countof(title), GetCurrentShapeName());
    wcscat_s(title, _countof(title), szBaseTitle);
    SetWindowTextW(hWnd, title);
}

void ShapeObjectsEditor::OnSize(HWND hWnd, HWND hwndToolBar) {
    if (!hwndToolBar) return;
    
    RECT rc, rw;
    GetClientRect(hWnd, &rc);
    GetWindowRect(hwndToolBar, &rw);
    MoveWindow(hwndToolBar, 0, 0, rc.right - rc.left, rw.bottom - rw.top, FALSE);
}