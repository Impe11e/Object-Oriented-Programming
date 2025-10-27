#include "shape_editor.h"
#include "Lab4.h"
#include <commctrl.h>

ShapeObjectsEditor::ShapeObjectsEditor() : currentEditor(nullptr), currentType(NO_EDITOR) {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        pcshape[i] = nullptr;
    }
    shapeCount = 0;
}

ShapeObjectsEditor::~ShapeObjectsEditor() {
    for (int i = 0; i < shapeCount; i++) {
        if (pcshape[i] != nullptr) {
            delete pcshape[i];
        }
    }
    delete currentEditor;
}

void PointEditor::OnLBdown(HWND hWnd) {
    if (*shapeCount >= arraySize) {
        MessageBox(hWnd, L"Масив заповнений!", L"Попередження", MB_OK);
        return;
    }

    POINT pt = GetMousePos(hWnd);
    pcshape[*shapeCount] = new PointShape();
    pcshape[*shapeCount]->Set(pt.x, pt.y, pt.x, pt.y);
    (*shapeCount)++;
    Invalidate(hWnd);
}

void PointEditor::OnLBup(HWND hWnd) {
}

void PointEditor::OnMouseMove(HWND hWnd) {
}

void LineEditor::OnLBdown(HWND hWnd) {
    startPoint = GetMousePos(hWnd);
    delete trail;
    trail = new LineShape();
    trail->Set(startPoint.x, startPoint.y, startPoint.x, startPoint.y);
    SetCapture(hWnd);
}

void LineEditor::OnLBup(HWND hWnd) {
    if (trail) {
        if (*shapeCount >= arraySize) {
            MessageBox(hWnd, L"Масив заповнений!", L"Попередження", MB_OK);
            delete trail;
            trail = nullptr;
            ReleaseCapture();
            return;
        }

        pcshape[*shapeCount] = trail;
        (*shapeCount)++;
        trail = nullptr;
        ReleaseCapture();
        Invalidate(hWnd);
    }
}

void LineEditor::OnMouseMove(HWND hWnd) {
    if (trail) {
        POINT pt = GetMousePos(hWnd);
        trail->Set(startPoint.x, startPoint.y, pt.x, pt.y);
        Invalidate(hWnd);
    }
}

void RectEditor::OnLBdown(HWND hWnd) {
    startPoint = GetMousePos(hWnd);
    delete trail;
    trail = new RectShape();
    trail->Set(startPoint.x, startPoint.y, startPoint.x, startPoint.y);
    SetCapture(hWnd);
}

void RectEditor::OnLBup(HWND hWnd) {
    if (trail) {
        if (*shapeCount >= arraySize) {
            MessageBox(hWnd, L"Масив заповнений!", L"Попередження", MB_OK);
            delete trail;
            trail = nullptr;
            ReleaseCapture();
            return;
        }

        pcshape[*shapeCount] = trail;
        (*shapeCount)++;
        trail = nullptr;
        ReleaseCapture();
        Invalidate(hWnd);
    }
}

void RectEditor::OnMouseMove(HWND hWnd) {
    if (trail) {
        POINT pt = GetMousePos(hWnd);
        
        int dx = abs(pt.x - startPoint.x);
        int dy = abs(pt.y - startPoint.y);
        
        trail->Set(startPoint.x - dx, startPoint.y - dy, 
                   startPoint.x + dx, startPoint.y + dy);
        Invalidate(hWnd);
    }
}

void EllipseEditor::OnLBdown(HWND hWnd) {
    startPoint = GetMousePos(hWnd);
    delete trail;
    trail = new EllipseShape();
    trail->Set(startPoint.x, startPoint.y, startPoint.x, startPoint.y);
    SetCapture(hWnd);
}

void EllipseEditor::OnLBup(HWND hWnd) {
    if (trail) {
        if (*shapeCount >= arraySize) {
            MessageBox(hWnd, L"Масив заповнений!", L"Попередження", MB_OK);
            delete trail;
            trail = nullptr;
            ReleaseCapture();
            return;
        }

        pcshape[*shapeCount] = trail;
        (*shapeCount)++;
        trail = nullptr;
        ReleaseCapture();
        Invalidate(hWnd);
    }
}

void EllipseEditor::OnMouseMove(HWND hWnd) {
    if (trail) {
        POINT pt = GetMousePos(hWnd);
        trail->Set(startPoint.x, startPoint.y, pt.x, pt.y);
        Invalidate(hWnd);
    }
}

void ShapeObjectsEditor::StartPointEditor(HWND hWnd) {
    delete currentEditor;
    currentEditor = new PointEditor(pcshape, &shapeCount, ARRAY_SIZE);
    currentType = POINT_EDITOR;
}

void ShapeObjectsEditor::StartLineEditor(HWND hWnd) {
    delete currentEditor;
    currentEditor = new LineEditor(pcshape, &shapeCount, ARRAY_SIZE);
    currentType = LINE_EDITOR;
}

void ShapeObjectsEditor::StartRectEditor(HWND hWnd) {
    delete currentEditor;
    currentEditor = new RectEditor(pcshape, &shapeCount, ARRAY_SIZE);
    currentType = RECT_EDITOR;
}

void ShapeObjectsEditor::StartEllipseEditor(HWND hWnd) {
    delete currentEditor;
    currentEditor = new EllipseEditor(pcshape, &shapeCount, ARRAY_SIZE);
    currentType = ELLIPSE_EDITOR;
}

const WCHAR* ShapeObjectsEditor::GetCurrentShapeName() const {
    const WCHAR* names[] = {L"", L"Крапка - ", L"Лінія - ", L"Прямокутник - ", L"Еліпс - "};
    return names[currentType];
}

void ShapeObjectsEditor::OnLBdown(HWND hWnd) {
    if (currentEditor)
        currentEditor->OnLBdown(hWnd);
}

void ShapeObjectsEditor::OnLBup(HWND hWnd) {
    if (currentEditor) {
        currentEditor->OnLBup(hWnd);
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

void ShapeObjectsEditor::OnMouseMove(HWND hWnd) {
    if (currentEditor)
        currentEditor->OnMouseMove(hWnd);
}

void ShapeObjectsEditor::OnPaint(HWND hWnd) {
    if (currentEditor) {
        currentEditor->OnPaint(hWnd);
    } else {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        
        for (int i = 0; i < shapeCount; i++) {
            if (pcshape[i]) {
                pcshape[i]->Show(hdc);
            }
        }
        
        EndPaint(hWnd, &ps);
    }
}

void ShapeObjectsEditor::OnInitMenuPopup(HWND hWnd, WPARAM wParam) {
    HMENU hMenu = (HMENU)wParam;
    CheckMenuItem(hMenu, ID_POINT, currentType == POINT_EDITOR ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu, ID_LINE, currentType == LINE_EDITOR ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu, ID_RECTANGLE, currentType == RECT_EDITOR ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu, ID_ELLIPSE, currentType == ELLIPSE_EDITOR ? MF_CHECKED : MF_UNCHECKED);
}

void ShapeObjectsEditor::OnToolButton(HWND hWnd, HWND hwndToolBar, int toolID)
{
    if (toolID == ID_POINT) {
        StartPointEditor(hWnd);
    }
    else if (toolID == ID_LINE) {
        StartLineEditor(hWnd);
    }
    else if (toolID == ID_RECTANGLE) {
        StartRectEditor(hWnd);
    }
    else if (toolID == ID_ELLIPSE) {
        StartEllipseEditor(hWnd);
    }
    
    SendMessage(hwndToolBar, TB_PRESSBUTTON, ID_POINT, MAKELONG(FALSE, 0));
    SendMessage(hwndToolBar, TB_PRESSBUTTON, ID_LINE, MAKELONG(FALSE, 0));
    SendMessage(hwndToolBar, TB_PRESSBUTTON, ID_RECTANGLE, MAKELONG(FALSE, 0));
    SendMessage(hwndToolBar, TB_PRESSBUTTON, ID_ELLIPSE, MAKELONG(FALSE, 0));
    
    SendMessage(hwndToolBar, TB_PRESSBUTTON, toolID, MAKELONG(TRUE, 0));
}

void ShapeObjectsEditor::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    LPNMHDR pnmh = (LPNMHDR)lParam;
    if (pnmh->code == TTN_NEEDTEXT) {
        LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
        switch (lpttt->hdr.idFrom) {
        case ID_POINT:
            lstrcpy(lpttt->szText, L"Крапка");
            break;
        case ID_LINE:
            lstrcpy(lpttt->szText, L"Лінія");
            break;
        case ID_RECTANGLE:
            lstrcpy(lpttt->szText, L"Прямокутник");
            break;
        case ID_ELLIPSE:
            lstrcpy(lpttt->szText, L"Еліпс");
            break;
        }
    }
}

void ShapeObjectsEditor::UpdateWindowTitle(HWND hWnd, LPCWSTR szBaseTitle)
{
    WCHAR title[256];
    const WCHAR* shapeName = GetCurrentShapeName();
    wcscpy_s(title, shapeName);
    wcscat_s(title, szBaseTitle);
    SetWindowTextW(hWnd, title);
}

void ShapeObjectsEditor::OnSize(HWND hWnd, HWND hwndToolBar)
{
    RECT rc, rw;
    if (hwndToolBar)
    {
        GetClientRect(hWnd, &rc);
        GetWindowRect(hwndToolBar, &rw); 
        MoveWindow(hwndToolBar, 0, 0,
            rc.right - rc.left, 
            rw.bottom - rw.top, FALSE);
    }
}