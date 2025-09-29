#pragma once
#include <windows.h>
#include "shape.h"
#include <vector>

class Editor {
protected:
    std::vector<Shape*>& shapes;
    POINT startPoint;
    Shape* trail;
    
public:
    Editor(std::vector<Shape*>& shapeList) : shapes(shapeList), trail(nullptr) {}
    virtual ~Editor() { delete trail; }
    
    virtual void OnLBdown(HWND hWnd) = 0;
    virtual void OnLBup(HWND hWnd) = 0;
    virtual void OnMouseMove(HWND hWnd) = 0;
    
    void OnPaint(HWND hWnd) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        
        for (auto shape : shapes) {
            shape->Show(hdc);
        }
        
        if (trail) {
            trail->Show(hdc);
        }
        
        EndPaint(hWnd, &ps);
    }
};

class PointEditor : public Editor {
public:
    PointEditor(std::vector<Shape*>& shapeList) : Editor(shapeList) {}
    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override {}
    void OnMouseMove(HWND hWnd) override {}
};

class LineEditor : public Editor {
public:
    LineEditor(std::vector<Shape*>& shapeList) : Editor(shapeList) {}
    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
    void OnMouseMove(HWND hWnd) override;
};

class RectEditor : public Editor {
public:
    RectEditor(std::vector<Shape*>& shapeList) : Editor(shapeList) {}
    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
    void OnMouseMove(HWND hWnd) override;
};

class EllipseEditor : public Editor {
public:
    EllipseEditor(std::vector<Shape*>& shapeList) : Editor(shapeList) {}
    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
    void OnMouseMove(HWND hWnd) override;
};