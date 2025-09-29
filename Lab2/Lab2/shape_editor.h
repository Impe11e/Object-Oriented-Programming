#pragma once
#include "editor.h"
#include <vector>

class ShapeObjectsEditor {
private:
    std::vector<Shape*> shapes;
    Editor* currentEditor;

public:
    ShapeObjectsEditor() : currentEditor(new PointEditor(shapes)) {}
    ~ShapeObjectsEditor() {
        for (auto shape : shapes) delete shape;
        delete currentEditor;
    }

    void StartPointEditor() { delete currentEditor; currentEditor = new PointEditor(shapes); }
    void StartLineEditor() { delete currentEditor; currentEditor = new LineEditor(shapes); }
    void StartRectEditor() { delete currentEditor; currentEditor = new RectEditor(shapes); }
    void StartEllipseEditor() { delete currentEditor; currentEditor = new EllipseEditor(shapes); }

    void OnLBdown(HWND hWnd) { currentEditor->OnLBdown(hWnd); }
    void OnLBup(HWND hWnd) { currentEditor->OnLBup(hWnd); }
    void OnMouseMove(HWND hWnd) { currentEditor->OnMouseMove(hWnd); }
    void OnPaint(HWND hWnd) { currentEditor->OnPaint(hWnd); }
    void OnInitMenuPopup(HWND hWnd, WPARAM wParam) {}
};