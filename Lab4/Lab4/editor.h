#pragma once
#include <windows.h>
#include "shape.h"

class Editor {
protected:
    Shape** pcshape;
    int* shapeCount;
    int arraySize;
    
    POINT GetMousePos(HWND hWnd) {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(hWnd, &pt);
        return pt;
    }
    
    void Invalidate(HWND hWnd) {
        InvalidateRect(hWnd, NULL, TRUE);
    }
    
public:
    Shape* trail = nullptr;
    
    Editor(Shape** shapeArray, int* count, int size) : pcshape(shapeArray), shapeCount(count), arraySize(size) {}
    virtual ~Editor() { delete trail; }
    
    virtual void OnLBdown(HWND hWnd) = 0;
    virtual void OnLBup(HWND hWnd) = 0;
    virtual void OnMouseMove(HWND hWnd) = 0;
    
    void OnPaint(HDC hdc) {
        if (trail) {
            HPEN hRedPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
            HPEN hOldPen = (HPEN)SelectObject(hdc, hRedPen);
            
            trail->Show(hdc);
            
            SelectObject(hdc, hOldPen);
            DeleteObject(hRedPen);
        }
    }
};

class PointEditor : public Editor {
public:
    PointEditor(Shape** shapeArray, int* count, int size) : Editor(shapeArray, count, size) {}
    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
    void OnMouseMove(HWND hWnd) override;
};

class LineEditor : public Editor {
private:    
    POINT startPoint;
public:
    LineEditor(Shape** shapeArray, int* count, int size) : Editor(shapeArray, count, size) {}
    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
    void OnMouseMove(HWND hWnd) override;
};

class RectEditor : public Editor {
private:
    POINT startPoint;
public:
    RectEditor(Shape** shapeArray, int* count, int size) : Editor(shapeArray, count, size) {}
    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
    void OnMouseMove(HWND hWnd) override;
};

class EllipseEditor : public Editor {
private:
    POINT startPoint;
public:
    EllipseEditor(Shape** shapeArray, int* count, int size) : Editor(shapeArray, count, size) {}
    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
    void OnMouseMove(HWND hWnd) override;
};
