#pragma once
#include "editor.h"
#include "resource.h"

#define ARRAY_SIZE 128

enum EditorType { POINT_EDITOR, LINE_EDITOR, RECT_EDITOR, ELLIPSE_EDITOR };

class ShapeObjectsEditor {
private:
    Shape* pcshape[ARRAY_SIZE];
    int shapeCount;
    Editor* currentEditor;
    EditorType currentType;
    
public:
    ShapeObjectsEditor() : currentType(POINT_EDITOR) {
        for (int i = 0; i < ARRAY_SIZE; i++) {
            pcshape[i] = nullptr;
        }
        shapeCount = 0;
        currentEditor = new PointEditor(pcshape, &shapeCount, ARRAY_SIZE);
    }
    
    ~ShapeObjectsEditor() {
        for (int i = 0; i < shapeCount; i++) {
            if (pcshape[i] != nullptr) {
                delete pcshape[i];
            }
        }
        delete currentEditor;
    }

    void StartPointEditor() { delete currentEditor; currentEditor = new PointEditor(pcshape, &shapeCount, ARRAY_SIZE); currentType = POINT_EDITOR; }
    void StartLineEditor() { delete currentEditor; currentEditor = new LineEditor(pcshape, &shapeCount, ARRAY_SIZE); currentType = LINE_EDITOR; }
    void StartRectEditor() { delete currentEditor; currentEditor = new RectEditor(pcshape, &shapeCount, ARRAY_SIZE); currentType = RECT_EDITOR; }
    void StartEllipseEditor() { delete currentEditor; currentEditor = new EllipseEditor(pcshape, &shapeCount, ARRAY_SIZE); currentType = ELLIPSE_EDITOR; }

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