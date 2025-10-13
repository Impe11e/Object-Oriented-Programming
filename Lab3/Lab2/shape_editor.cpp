#include "shape_editor.h"
#include "Lab2.h"

void PointEditor::OnLBdown(HWND hWnd) {
    if (*shapeCount >= arraySize) {
        MessageBox(hWnd, L"Масив заповнен!", L"Попередження", MB_OK);
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
            MessageBox(hWnd, L"Масив заповнен!", L"Попередження", MB_OK);
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
            MessageBox(hWnd, L"Масив заповнен!", L"Попередження", MB_OK);
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
            MessageBox(hWnd, L"Масив заповнен!", L"Попередження", MB_OK);
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