#include "my_editor.h"
#include "Lab5.h"
#include "my_table.h"
#include "point.h"
#include "line.h"
#include "rectangle.h"
#include "ellipse.h"
#include "linewithcircles.h"
#include "cube.h"
#include <string.h>
#include <wchar.h>

#pragma comment(lib, "Comctl32.lib")

MyEditor* MyEditor::p_instance = nullptr;

static void TableHoverForward(int idx) {
    MyEditor* e = MyEditor::getInstance();
    if (e) e->OnTableHover(idx);
}
static void TableRemoveForward(int idx) {
    MyEditor* e = MyEditor::getInstance();
    if (e) e->OnTableRemove(idx);
}

MyEditor* MyEditor::getInstance() {
    if (!p_instance)
        p_instance = new MyEditor();
    return p_instance;
}

void MyEditor::Cleanup() {
    if (p_instance) {
        delete p_instance;
        p_instance = nullptr;
    }
}

void MyEditor::EnsureCapacity() {
    if (!pcshape) {
        capacity = INITIAL_CAPACITY;
        pcshape = new Shape*[capacity];
        memset(pcshape, 0, sizeof(Shape*) * capacity);
        shapeCount = 0;
        return;
    }
    if (shapeCount < capacity) return;
    int newCap = capacity * 2;
    Shape** newArr = new Shape*[newCap];
    memset(newArr, 0, sizeof(Shape*) * newCap);
    for (int i = 0; i < shapeCount; ++i) newArr[i] = pcshape[i];
    delete[] pcshape;
    pcshape = newArr;
    capacity = newCap;
}

MyEditor::MyEditor()
    : pcshape(nullptr),
      shapeCount(0),
      capacity(0),
      currentShape(nullptr),
      currentType(NO_EDITOR),
      hwndToolBar(nullptr),
      mainHwnd(nullptr),
      hoveredIndex(-1),
      fileOut(nullptr)
{
    fopen_s(&fileOut, "shapes.txt", "wt");
    if (fileOut) {
        fprintf(fileOut, "Назва\tx1\ty1\tx2\ty2\n");
        fclose(fileOut);
        fileOut = nullptr;
    }

    MyTable::SetHoverCallback(TableHoverForward);
    MyTable::SetRemoveCallback(TableRemoveForward);
}

MyEditor::~MyEditor() {
    if (pcshape) {
        for (int i = 0; i < shapeCount; ++i) delete pcshape[i];
        delete[] pcshape;
        pcshape = nullptr;
    }
    delete currentShape;
    currentShape = nullptr;
}

void MyEditor::WriteShapeToFile(Shape* shape) {
    if (!shape) return;
    
    fopen_s(&fileOut, "shapes.txt", "at");
    if (!fileOut) return;

    const char* shapeName = shape->GetName();
    
    long x1, y1, x2, y2;
    shape->Get(x1, y1, x2, y2);
    
    fprintf(fileOut, "%s\t%ld\t%ld\t%ld\t%ld\n", shapeName, x1, y1, x2, y2);
    fclose(fileOut);
    fileOut = nullptr;
}

void MyEditor::Start(Shape* prototype) {
    if (!prototype) {
        currentType = NO_EDITOR;
        return;
    }
    
    EnsureCapacity();

    if (dynamic_cast<PointShape*>(prototype)) {
        currentType = POINT_EDITOR;
    } else if (dynamic_cast<LineOOShape*>(prototype)) {
        currentType = LINEOO_EDITOR;
    } else if (dynamic_cast<CubeShape*>(prototype)) {
        currentType = CUBE_EDITOR;
    } else if (dynamic_cast<LineShape*>(prototype) && !dynamic_cast<LineOOShape*>(prototype)) {
        currentType = LINE_EDITOR;
    } else if (dynamic_cast<RectShape*>(prototype)) {
        currentType = RECT_EDITOR;
    } else if (dynamic_cast<EllipseShape*>(prototype)) {
        currentType = ELLIPSE_EDITOR;
    } else {
        currentType = NO_EDITOR;
    }

    delete currentShape;
    currentShape = prototype;
}

void MyEditor::OnLBdown(HWND hWnd) {
    if (!currentShape) return;
    GetCursorPos(&start);
    ScreenToClient(hWnd, &start);
    currentShape->Set(start.x, start.y, start.x, start.y);
    SetCapture(hWnd);
}

void MyEditor::OnLBup(HWND hWnd) {
    if (!currentShape) return;
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    currentShape->Set(start.x, start.y, pt.x, pt.y);

    EnsureCapacity();
    pcshape[shapeCount++] = currentShape;

    WriteShapeToFile(currentShape);
    
    const char* shapeName = currentShape->GetName();
    
    long x1, y1, x2, y2;
    currentShape->Get(x1, y1, x2, y2);
    
    MyTable::Add(shapeName, x1, y1, x2, y2);

    if (pcshape[shapeCount - 1])
        currentShape = pcshape[shapeCount - 1]->Clone();
    else
        currentShape = nullptr;

    ReleaseCapture();
    InvalidateRect(hWnd, NULL, FALSE);
}

void MyEditor::OnMouseMove(HWND hWnd) {
    if (!currentShape) return;
    if ((GetKeyState(VK_LBUTTON) & 0x8000) == 0) return;

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);

    HDC hdc = GetDC(hWnd);
    
    int oldROP = SetROP2(hdc, R2_NOTXORPEN);

    currentShape->ShowTrail(hdc);

    currentShape->Set(start.x, start.y, pt.x, pt.y);
    
    currentShape->ShowTrail(hdc);

    SetROP2(hdc, oldROP);
    ReleaseDC(hWnd, hdc);
}

void MyEditor::OnPaint(HWND hWnd, HDC hdc) {
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

    HBRUSH hbrBk = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    FillRect(hdcMem, &rcClient, hbrBk);
    DeleteObject(hbrBk);

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdcMem, hPen);

    for (int i = 0; i < shapeCount; ++i) {
        if (pcshape[i]) pcshape[i]->Show(hdcMem);
    }

    if (hoveredIndex >= 0 && hoveredIndex < shapeCount && pcshape[hoveredIndex]) {
        HPEN hRedPen = CreatePen(PS_SOLID, 2, RGB(255,0,0));
        HPEN hPrev = (HPEN)SelectObject(hdcMem, hRedPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
        pcshape[hoveredIndex]->Show(hdcMem);
        SelectObject(hdcMem, hOldBrush);
        SelectObject(hdcMem, hPrev);
        DeleteObject(hRedPen);
    }

    if (currentShape) currentShape->Show(hdcMem);

    BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void MyEditor::AttachToolbar(HWND hwnd) {
    hwndToolBar = hwnd;
}

void MyEditor::AttachMainWindow(HWND hwnd) {
    mainHwnd = hwnd;
}

void MyEditor::OnToolButtonClick(WPARAM wParam) {
    int buttonID = LOWORD(wParam);
    const int ids[] = { ID_POINT, ID_LINE, ID_RECTANGLE, ID_ELLIPSE, ID_LINEOO, ID_CUBE };
    for (int i = 0; i < (int)(sizeof(ids)/sizeof(ids[0])); ++i) {
        SendMessage(hwndToolBar, TB_PRESSBUTTON, ids[i], MAKELONG(FALSE, 0));
    }
    SendMessage(hwndToolBar, TB_PRESSBUTTON, buttonID, MAKELONG(TRUE, 0));
}

void MyEditor::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    LPNMHDR pnmh = (LPNMHDR)lParam;
    if (!pnmh) return;
    if (pnmh->code == TTN_NEEDTEXT) {
        LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
        switch (lpttt->hdr.idFrom) {
        case ID_POINT: wcscpy_s(lpttt->szText, _countof(lpttt->szText), L"Точка"); break;
        case ID_LINE: wcscpy_s(lpttt->szText, _countof(lpttt->szText), L"Лінія"); break;
        case ID_RECTANGLE: wcscpy_s(lpttt->szText, _countof(lpttt->szText), L"Прямокутник"); break;
        case ID_ELLIPSE: wcscpy_s(lpttt->szText, _countof(lpttt->szText), L"Еліпс"); break;
        case ID_LINEOO: wcscpy_s(lpttt->szText, _countof(lpttt->szText), L"Лінія з кружечками"); break;
        case ID_CUBE: wcscpy_s(lpttt->szText, _countof(lpttt->szText), L"Куб"); break;
        default: wcscpy_s(lpttt->szText, _countof(lpttt->szText), L""); break;
        }
    }
}

void MyEditor::OnInitMenuPopup(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    HMENU hMenu = (HMENU)wParam;
    struct Item { int id; EditorType type; };
    const Item items[] = {
        { ID_POINT, POINT_EDITOR }, { ID_LINE, LINE_EDITOR }, { ID_RECTANGLE, RECT_EDITOR },
        { ID_ELLIPSE, ELLIPSE_EDITOR }, { ID_LINEOO, LINEOO_EDITOR }, { ID_CUBE, CUBE_EDITOR }
    };
    for (const Item& it : items) {
        UINT uCheck = (currentType == it.type) ? MF_CHECKED : MF_UNCHECKED;
        CheckMenuItem(hMenu, it.id, MF_BYCOMMAND | uCheck);
    }
}

void MyEditor::UpdateWindowTitle(HWND hWnd, LPCWSTR szBaseTitle) {
    WCHAR title[256] = {0};
    const WCHAR* names[] = { L"", L"Точка - ", L"Лінія - ", L"Прямокутник - ", L"Еліпс - ", L"Лінія з кружечками - ", L"Куб - " };
    const WCHAR* prefix = names[static_cast<int>(currentType)];
    wcscpy_s(title, _countof(title), prefix);
    wcscat_s(title, _countof(title), szBaseTitle);
    SetWindowTextW(hWnd, title);
}

void MyEditor::OnSize(HWND hWnd, HWND hwndToolBar_) {
    if (!hwndToolBar_) return;
    RECT rc, rw;
    GetClientRect(hWnd, &rc);
    GetWindowRect(hwndToolBar_, &rw);
    MoveWindow(hwndToolBar_, 0, 0, rc.right - rc.left, rw.bottom - rw.top, FALSE);
}

void MyEditor::OnTableHover(int index) {
    if (index < 0 || index >= shapeCount) {
        hoveredIndex = -1;
    } else {
        hoveredIndex = index;
    }
    if (mainHwnd) InvalidateRect(mainHwnd, NULL, FALSE);
}

void MyEditor::OnTableRemove(int index) {
    if (index < 0 || index >= shapeCount) return;
    delete pcshape[index];
    for (int i = index; i < shapeCount - 1; ++i) pcshape[i] = pcshape[i+1];
    --shapeCount;
    pcshape[shapeCount] = nullptr;

    if (mainHwnd) InvalidateRect(mainHwnd, NULL, FALSE);
}
