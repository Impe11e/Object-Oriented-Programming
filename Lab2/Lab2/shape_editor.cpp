#include "shape_editor.h"
#include "Lab2.h"

void PointEditor::OnLBdown(HWND hWnd) {
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    
    shapes.push_back(new PointShape());
    shapes.back()->Set(pt.x, pt.y, pt.x, pt.y);
    InvalidateRect(hWnd, NULL, TRUE);
}


void LineEditor::OnLBdown(HWND hWnd) {
    GetCursorPos(&startPoint);
    ScreenToClient(hWnd, &startPoint);
    trail = new LineShape();
    trail->Set(startPoint.x, startPoint.y, startPoint.x, startPoint.y);
    SetCapture(hWnd);
}

void LineEditor::OnLBup(HWND hWnd) {
    if (trail) {
        shapes.push_back(trail);
        trail = nullptr;
        ReleaseCapture();
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

void LineEditor::OnMouseMove(HWND hWnd) {
    if (trail) {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(hWnd, &pt);
        trail->Set(startPoint.x, startPoint.y, pt.x, pt.y);
        InvalidateRect(hWnd, NULL, TRUE);
    }
}


void RectEditor::OnLBdown(HWND hWnd) {
    GetCursorPos(&startPoint);
    ScreenToClient(hWnd, &startPoint);
    trail = new RectShape();
    trail->Set(startPoint.x, startPoint.y, startPoint.x, startPoint.y);
    SetCapture(hWnd);
}

void RectEditor::OnLBup(HWND hWnd) {
    if (trail) {
        shapes.push_back(trail);
        trail = nullptr;
        ReleaseCapture();
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

void RectEditor::OnMouseMove(HWND hWnd) {
    if (trail) {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(hWnd, &pt);
        trail->Set(startPoint.x, startPoint.y, pt.x, pt.y);
        InvalidateRect(hWnd, NULL, TRUE);
    }
}


void EllipseEditor::OnLBdown(HWND hWnd) {
    GetCursorPos(&startPoint);
    ScreenToClient(hWnd, &startPoint);
    trail = new EllipseShape();
    trail->Set(startPoint.x, startPoint.y, startPoint.x, startPoint.y);
    SetCapture(hWnd);
}

void EllipseEditor::OnLBup(HWND hWnd) {
    if (trail) {
        shapes.push_back(trail);
        trail = nullptr;
        ReleaseCapture();
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

void EllipseEditor::OnMouseMove(HWND hWnd) {
    if (trail) {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(hWnd, &pt);
        
        long deltaX = abs(pt.x - startPoint.x);
        long deltaY = abs(pt.y - startPoint.y);
        trail->Set(startPoint.x - deltaX, startPoint.y - deltaY, 
                   startPoint.x + deltaX, startPoint.y + deltaY);
        InvalidateRect(hWnd, NULL, TRUE);
    }
}