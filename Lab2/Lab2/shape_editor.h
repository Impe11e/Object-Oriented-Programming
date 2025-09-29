#pragma once
#include "editor.h"
#include "resource.h"
#include <vector>

enum EditorType { POINT_EDITOR, LINE_EDITOR, RECT_EDITOR, ELLIPSE_EDITOR };

class ShapeObjectsEditor {
private:
    std::vector<Shape*> shapes;
    Editor* currentEditor;
    EditorType currentType;
    
public:
    ShapeObjectsEditor() : currentEditor(new PointEditor(shapes)), currentType(POINT_EDITOR) {}
    ~ShapeObjectsEditor() {
        for (auto shape : shapes) delete shape;
        delete currentEditor;
    }

    void StartPointEditor() { delete currentEditor; currentEditor = new PointEditor(shapes); currentType = POINT_EDITOR; }
    void StartLineEditor() { delete currentEditor; currentEditor = new LineEditor(shapes); currentType = LINE_EDITOR; }
    void StartRectEditor() { delete currentEditor; currentEditor = new RectEditor(shapes); currentType = RECT_EDITOR; }
    void StartEllipseEditor() { delete currentEditor; currentEditor = new EllipseEditor(shapes); currentType = ELLIPSE_EDITOR; }

    void OnLBdown(HWND hWnd) { currentEditor->OnLBdown(hWnd); }
    void OnLBup(HWND hWnd) { currentEditor->OnLBup(hWnd); }
    void OnMouseMove(HWND hWnd) { currentEditor->OnMouseMove(hWnd); }
    void OnPaint(HWND hWnd) { currentEditor->OnPaint(hWnd); }
    
    void OnInitMenuPopup(HWND hWnd, WPARAM wParam) {
        HMENU hMenu = (HMENU)wParam;
        CheckMenuItem(hMenu, ID_POINT, currentType == POINT_EDITOR ? MF_CHECKED : MF_UNCHECKED);
        CheckMenuItem(hMenu, ID_LINE, currentType == LINE_EDITOR ? MF_CHECKED : MF_UNCHECKED);
        CheckMenuItem(hMenu, ID_RECTANGLE, currentType == RECT_EDITOR ? MF_CHECKED : MF_UNCHECKED);
        CheckMenuItem(hMenu, ID_ELLIPSE, currentType == ELLIPSE_EDITOR ? MF_CHECKED : MF_UNCHECKED);
    }
};