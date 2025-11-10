#pragma once

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
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
    static MyEditor* p_instance;
    
    Shape** pcshape;     
    int shapeCount;      
    int capacity;        

    Shape* currentShape; 
    EditorType currentType; 
    POINT start;         
    HWND hwndToolBar;
    HWND mainHwnd;       
    int hoveredIndex;    
    FILE* fileOut;

    void EnsureCapacity();
    void WriteShapeToFile(Shape* shape);

    MyEditor();
    MyEditor(const MyEditor&);
    MyEditor& operator=(MyEditor&);

public:
    ~MyEditor();

    static MyEditor* getInstance();
    static void Cleanup();

    void Start(Shape* prototype);

    void OnLBdown(HWND hWnd);
    void OnLBup(HWND hWnd);
    void OnMouseMove(HWND hWnd);

    void OnPaint(HWND hWnd, HDC hdc);

    void AttachToolbar(HWND hwnd);
    void AttachMainWindow(HWND hwnd);

    void OnToolButtonClick(WPARAM wParam);
    void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void OnInitMenuPopup(HWND hWnd, WPARAM wParam, LPARAM lParam = 0);
    void UpdateWindowTitle(HWND hWnd, LPCWSTR szTitle);
    void OnSize(HWND hWnd, HWND hwndToolBar);

    int GetShapeCount() const { return shapeCount; }
    Shape* GetShape(int index) const { 
        if (index >= 0 && index < shapeCount) return pcshape[index];
        return nullptr;
    }
    EditorType GetCurrentType() const { return currentType; }

    void OnTableHover(int index);
    void OnTableRemove(int index);
};