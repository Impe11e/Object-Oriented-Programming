#pragma once
#include "editor.h"
#include "resource.h"
#include <commctrl.h> 

#define ARRAY_SIZE 129

enum EditorType { NO_EDITOR, POINT_EDITOR, LINE_EDITOR, RECT_EDITOR, ELLIPSE_EDITOR };

class ShapeObjectsEditor {
private:
    Shape* pcshape[ARRAY_SIZE];
    int shapeCount;
    Editor* currentEditor;
    EditorType currentType;
    
public:
    ShapeObjectsEditor() : currentEditor(nullptr), currentType(NO_EDITOR) {
        for (int i = 0; i < ARRAY_SIZE; i++) {
            pcshape[i] = nullptr;
        }
        shapeCount = 0;
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

    const WCHAR* GetCurrentShapeName() const {
        if (currentType == NO_EDITOR) return L"";        
        if (currentType == POINT_EDITOR) return L"Крапка - ";
        if (currentType == LINE_EDITOR) return L"Лінія - ";
        if (currentType == RECT_EDITOR) return L"Прямокутник - ";
        if (currentType == ELLIPSE_EDITOR) return L"Еліпс - ";
        return L"";
    }

    void OnLBdown(HWND hWnd) { if (currentEditor) currentEditor->OnLBdown(hWnd); }
    void OnLBup(HWND hWnd) { if (currentEditor) currentEditor->OnLBup(hWnd); }
    void OnMouseMove(HWND hWnd) { if (currentEditor) currentEditor->OnMouseMove(hWnd); }
    void OnPaint(HWND hWnd) { 
        if (currentEditor) {
            currentEditor->OnPaint(hWnd); 
        } else {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            
            for (int i = 0; i < shapeCount; i++) {
                if (pcshape[i]) {
                    pcshape[i]->Show(hdc);
                }
            }
            
            EndPaint(hWnd, &ps);
        }
    }
    
    void OnInitMenuPopup(HWND hWnd, WPARAM wParam) {
        HMENU hMenu = (HMENU)wParam;
        CheckMenuItem(hMenu, ID_POINT, currentType == POINT_EDITOR ? MF_CHECKED : MF_UNCHECKED);
        CheckMenuItem(hMenu, ID_LINE, currentType == LINE_EDITOR ? MF_CHECKED : MF_UNCHECKED);
        CheckMenuItem(hMenu, ID_RECTANGLE, currentType == RECT_EDITOR ? MF_CHECKED : MF_UNCHECKED);
        CheckMenuItem(hMenu, ID_ELLIPSE, currentType == ELLIPSE_EDITOR ? MF_CHECKED : MF_UNCHECKED);
    }
    
    LRESULT OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam) {
        LPNMHDR lpnmhdr = (LPNMHDR)lParam;
        if (lpnmhdr->code == TTN_NEEDTEXT) {
            LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
            switch (lpttt->hdr.idFrom) {
            case ID_POINT: lstrcpy(lpttt->szText, L"Крапка"); break;
            case ID_LINE: lstrcpy(lpttt->szText, L"Лінія"); break;
            case ID_RECTANGLE: lstrcpy(lpttt->szText, L"Прямокутник"); break;
            case ID_ELLIPSE: lstrcpy(lpttt->szText, L"Еліпс"); break;
            default: lstrcpy(lpttt->szText, L"Щось невідоме"); break;
            }
            return 0;
        }
        
        return 0;
    }
};
