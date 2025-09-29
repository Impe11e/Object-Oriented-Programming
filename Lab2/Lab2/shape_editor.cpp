#include "shape_editor.h"
#include "Lab2.h"

// PointEditor - максимально простой!
void PointEditor::OnLBdown(HWND hWnd) {
    POINT pt = GetMousePos(hWnd);
    shapes.push_back(new PointShape());
    shapes.back()->Set(pt.x, pt.y, pt.x, pt.y);
    Invalidate(hWnd);
}

void PointEditor::OnLBup(HWND hWnd) {
    // Для точки ничего не делаем
}

void PointEditor::OnMouseMove(HWND hWnd) {
    // Для точки ничего не делаем
}

// LineEditor - лаконичный с trail системой
void LineEditor::OnLBdown(HWND hWnd) {
    startPoint = GetMousePos(hWnd);
    delete trail;
    trail = new LineShape();
    trail->Set(startPoint.x, startPoint.y, startPoint.x, startPoint.y);
    SetCapture(hWnd);
}

void LineEditor::OnLBup(HWND hWnd) {
    if (trail) {
        shapes.push_back(trail);
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

// RectEditor - сокращен в 3 раза!
void RectEditor::OnLBdown(HWND hWnd) {
    startPoint = GetMousePos(hWnd);
    delete trail;
    trail = new RectShape();
    trail->Set(startPoint.x, startPoint.y, startPoint.x, startPoint.y);
    SetCapture(hWnd);
}

void RectEditor::OnLBup(HWND hWnd) {
    if (trail) {
        shapes.push_back(trail);
        trail = nullptr;
        ReleaseCapture();
        Invalidate(hWnd);
    }
}

void RectEditor::OnMouseMove(HWND hWnd) {
    if (trail) {
        POINT pt = GetMousePos(hWnd);
        trail->Set(startPoint.x, startPoint.y, pt.x, pt.y);
        Invalidate(hWnd);
    }
}

// EllipseEditor - сокращен в 3 раза с учетом варианта 28!
void EllipseEditor::OnLBdown(HWND hWnd) {
    startPoint = GetMousePos(hWnd);
    delete trail;
    trail = new EllipseShape();
    trail->Set(startPoint.x, startPoint.y, startPoint.x, startPoint.y);
    SetCapture(hWnd);
}

void EllipseEditor::OnLBup(HWND hWnd) {
    if (trail) {
        shapes.push_back(trail);
        trail = nullptr;
        ReleaseCapture();
        Invalidate(hWnd);
    }
}

void EllipseEditor::OnMouseMove(HWND hWnd) {
    if (trail) {
        POINT pt = GetMousePos(hWnd);
        
        // Для эллипса от центра к углу (вариант 28)
        long deltaX = abs(pt.x - startPoint.x);
        long deltaY = abs(pt.y - startPoint.y);
        trail->Set(startPoint.x - deltaX, startPoint.y - deltaY, 
                   startPoint.x + deltaX, startPoint.y + deltaY);
        Invalidate(hWnd);
    }
}
