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
        InvalidateRect(hWnd, NULL, FALSE);
    }
    
    bool CheckArrayOverflow(HWND hWnd) {
        if (*shapeCount >= arraySize) {
            MessageBox(hWnd, L"Масив заповнений!", L"Попередження", MB_OK);
            return true;
        }
        return false;
    }
    
    void AddShapeToArray() {
        if (trail) {
            pcshape[(*shapeCount)++] = trail;
            trail = nullptr;
        }
    }
    
public:
    Shape* trail = nullptr;
    
    Editor(Shape** shapeArray, int* count, int size) 
        : pcshape(shapeArray), shapeCount(count), arraySize(size) {}
    
    virtual ~Editor() { delete trail; }
    
    virtual void OnLBdown(HWND hWnd) = 0;
    virtual void OnLBup(HWND hWnd) = 0;
    virtual void OnMouseMove(HWND hWnd) = 0;
    
    void OnPaint(HDC hdc) {
        if (trail) {
            HPEN hRedPen = CreatePen(PS_DOT, 1, RGB(255, 0, 0));
            HPEN hOldPen = (HPEN)SelectObject(hdc, hRedPen);
            
            trail->Show(hdc);
            
            SelectObject(hdc, hOldPen);
            DeleteObject(hRedPen);
        }
    }
};

class TwoPointEditor : public Editor {
protected:
    POINT startPoint;
    
    virtual Shape* CreateShape() = 0;
    
    virtual void UpdateShapeCoordinates(POINT currentPoint) {
        trail->Set(startPoint.x, startPoint.y, currentPoint.x, currentPoint.y);
    }
    
public:
    TwoPointEditor(Shape** shapeArray, int* count, int size) 
        : Editor(shapeArray, count, size) {}
    
    void OnLBdown(HWND hWnd) override {
        startPoint = GetMousePos(hWnd);
        delete trail;
        trail = CreateShape();
        trail->Set(startPoint.x, startPoint.y, startPoint.x, startPoint.y);
        SetCapture(hWnd);
    }
    
    void OnLBup(HWND hWnd) override {
        if (!trail) return;
        
        if (CheckArrayOverflow(hWnd)) {
            delete trail;
            trail = nullptr;
        } else {
            AddShapeToArray();
        }
        
        ReleaseCapture();
        Invalidate(hWnd);
    }
    
    void OnMouseMove(HWND hWnd) override {
        if (trail) {
            POINT currentPoint = GetMousePos(hWnd);
            UpdateShapeCoordinates(currentPoint);
            Invalidate(hWnd);
        }
    }
};

class PointEditor : public Editor {
public:
    PointEditor(Shape** shapeArray, int* count, int size) 
        : Editor(shapeArray, count, size) {}
    
    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override {}
    void OnMouseMove(HWND hWnd) override {}
};

class LineEditor : public TwoPointEditor {
protected:
    Shape* CreateShape() override { return new LineShape(); }
    
public:
    LineEditor(Shape** shapeArray, int* count, int size) 
        : TwoPointEditor(shapeArray, count, size) {}
};

class RectEditor : public TwoPointEditor {
protected:
    Shape* CreateShape() override { return new RectShape(); }
    
    void UpdateShapeCoordinates(POINT currentPoint) override {
        int dx = abs(currentPoint.x - startPoint.x);
        int dy = abs(currentPoint.y - startPoint.y);
        trail->Set(startPoint.x - dx, startPoint.y - dy, 
                   startPoint.x + dx, startPoint.y + dy);
    }
    
public:
    RectEditor(Shape** shapeArray, int* count, int size) 
        : TwoPointEditor(shapeArray, count, size) {}
};

class EllipseEditor : public TwoPointEditor {
protected:
    Shape* CreateShape() override { return new EllipseShape(); }
    
public:
    EllipseEditor(Shape** shapeArray, int* count, int size) 
        : TwoPointEditor(shapeArray, count, size) {}
};

class LineOOEditor : public TwoPointEditor {
protected:
    Shape* CreateShape() override { return new LineOOShape(); }
    
public:
    LineOOEditor(Shape** shapeArray, int* count, int size) 
        : TwoPointEditor(shapeArray, count, size) {}
};

class CubeEditor : public TwoPointEditor {
protected:
    Shape* CreateShape() override { return new CubeShape(); }
    
public:
    CubeEditor(Shape** shapeArray, int* count, int size) 
        : TwoPointEditor(shapeArray, count, size) {}
};
