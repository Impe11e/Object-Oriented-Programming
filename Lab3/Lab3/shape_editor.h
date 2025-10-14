#pragma once
#include "editor.h"
#include "resource.h"

#define ARRAY_SIZE 129

enum EditorType { NO_EDITOR, POINT_EDITOR, LINE_EDITOR, RECT_EDITOR, ELLIPSE_EDITOR };

class ShapeObjectsEditor {
private:
    Shape* pcshape[ARRAY_SIZE];
    int shapeCount;
    Editor* currentEditor;
    EditorType currentType;
    
public:
    ShapeObjectsEditor();
    ~ShapeObjectsEditor();

    void StartPointEditor(HWND hWnd);
    void StartLineEditor(HWND hWnd);
    void StartRectEditor(HWND hWnd);
    void StartEllipseEditor(HWND hWnd);

    const WCHAR* GetCurrentShapeName() const;

    void OnLBdown(HWND hWnd);
    void OnLBup(HWND hWnd);
    void OnMouseMove(HWND hWnd);
    void OnPaint(HWND hWnd);
    void OnInitMenuPopup(HWND hWnd, WPARAM wParam);
    void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
    
    void UpdateWindowTitle(HWND hWnd, LPCWSTR szTitle);
    void OnToolButton(HWND hWnd, HWND hwndToolBar, int toolID);
    void OnSize(HWND hWnd, HWND hwndToolBar);
};
