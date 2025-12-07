#pragma once
#include <windows.h>
#include <CommCtrl.h>
#include <vector>
#include <string>

#pragma comment(lib, "Comctl32.lib")

class MyTableClass
{
public:
    MyTableClass(HWND hParentWnd, HINSTANCE hInst);
    ~MyTableClass();

    bool LoadFromFile(const wchar_t* filename);
    bool SaveToFile(const wchar_t* filename);
    bool SaveToFileAs(const wchar_t* filename);
    
    void BeginEdit(int row, int col);
    void EndEdit(bool save);
    void Undo();
    
    void OnSize(int width, int height);
    void OnNotify(LPARAM lParam);
    void OnKeyDown(WPARAM wParam);  
    
    bool HasData() const { return !m_data.empty(); }
    const wchar_t* GetCurrentFileName() const { return m_currentFileName.c_str(); }
    void SetCurrentFileName(const wchar_t* filename) { m_currentFileName = filename; }
    bool IsModified() const { return m_modified; }
    void SetModified(bool modified) { m_modified = modified; }
    HWND GetListViewHandle() const { return m_hListView; }

private:
    std::vector<std::vector<std::wstring>> m_data;
    
    HWND m_hParentWnd;
    HWND m_hListView;
    HWND m_hEditWnd;
    HINSTANCE m_hInst;
    
    int m_editRow;
    int m_editCol;
    bool m_isEditing;
    WNDPROC m_oldListViewProc;
    
    struct UndoState {
        int row;
        int col;
        std::wstring oldValue;
        std::wstring newValue;
    };
    std::vector<UndoState> m_undoStack;
    
    std::wstring m_currentFileName;
    bool m_modified;
    
    void CreateListView();
    void InitializeColumns();
    void FillListView();
    void CreateEditControl(const RECT& cellRect, const std::wstring& text);
    void DestroyEditControl();
    void SaveEditState();
    void GetSubItemRect(int row, int col, RECT* rect);
    
    static LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, 
                                              LPARAM lParam, UINT_PTR uIdSubclass, 
                                              DWORD_PTR dwRefData);
    
    static LRESULT CALLBACK ListViewSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, 
                                                   LPARAM lParam, UINT_PTR uIdSubclass, 
                                                   DWORD_PTR dwRefData);
};
