#pragma once
#include <windows.h>
#include "shape.h"
#include <vector>

class Editor {
protected:
    std::vector<Shape*>& shapes;
    
    // Базовый метод для получения координат (убирает дублирование!)
    POINT GetMousePos(HWND hWnd) {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(hWnd, &pt);
        return pt;
    }
    
    // Базовый метод для обновления экрана (убирает дублирование!)
    void Invalidate(HWND hWnd) {
        InvalidateRect(hWnd, NULL, TRUE);
    }
    
public:
    Shape* trail = nullptr;
    
    Editor(std::vector<Shape*>& shapeList) : shapes(shapeList) {}
    virtual ~Editor() { delete trail; }
    
    // ОРИГИНАЛЬНЫЕ ИМЕНА МЕТОДОВ
    virtual void OnLBdown(HWND hWnd) = 0;
    virtual void OnLBup(HWND hWnd) = 0;
    virtual void OnMouseMove(HWND hWnd) = 0;
    
    // Единый OnPaint для всех
    void OnPaint(HWND hWnd) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        
        for (auto shape : shapes) shape->Show(hdc);
        if (trail) trail->Show(hdc);
        
        EndPaint(hWnd, &ps);
    }
};

class PointEditor : public Editor {
public:
    PointEditor(std::vector<Shape*>& shapeList) : Editor(shapeList) {}
    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
    void OnMouseMove(HWND hWnd) override;
};

class LineEditor : public Editor {
private:    
    POINT startPoint;
public:
    LineEditor(std::vector<Shape*>& shapeList) : Editor(shapeList) {}
    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
    void OnMouseMove(HWND hWnd) override;
};

class RectEditor : public Editor {
private:
    POINT startPoint;
public:
    RectEditor(std::vector<Shape*>& shapeList) : Editor(shapeList) {}
    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
    void OnMouseMove(HWND hWnd) override;
};

class EllipseEditor : public Editor {
private:
    POINT startPoint;
public:
    EllipseEditor(std::vector<Shape*>& shapeList) : Editor(shapeList) {}
    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
    void OnMouseMove(HWND hWnd) override;
};