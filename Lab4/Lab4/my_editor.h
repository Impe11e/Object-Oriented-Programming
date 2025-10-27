#pragma once

#include <windows.h>
#include <commctrl.h>
#include "shape.h"
#include "resource.h"

#define INITIAL_CAPACITY 16

enum EditorType {
    NO_EDITOR = 0,
    POINT_EDITOR,
    LINE_EDITOR,
    RECT_EDITOR,
    ELLIPSE_EDITOR,
    LINEOO_EDITOR,
    CUBE_EDITOR
};

class MyEditor {
private:
    Shape** pcshape;     
    int shapeCount;      
    int capacity;        

    Shape* currentShape; 
    EditorType currentType; 
    int currentToolID;   
    POINT start;         
    POINT end;           
    HWND hwndToolBar;    

    void EnsureCapacity();

public:
    MyEditor();
    ~MyEditor();

    void Start(Shape* prototype);

    void OnLBdown(HWND hWnd);
    void OnLBup(HWND hWnd);
    void OnMouseMove(HWND hWnd);

    void OnPaint(HWND hWnd, HDC hdc);

    void OnToolbarCreate(HWND hWnd, HINSTANCE hInst);
    void OnToolButtonClick(WPARAM wParam);
    void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void OnInitMenuPopup(HWND hWnd, WPARAM wParam);
    void UpdateWindowTitle(HWND hWnd, LPCWSTR szTitle);
    void OnSize(HWND hWnd, HWND hwndToolBar);

    int GetShapeCount() const { return shapeCount; }
};