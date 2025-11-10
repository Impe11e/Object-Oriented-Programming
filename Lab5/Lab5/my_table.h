#pragma once

#include "framework.h"
#include "resource.h"
#include <commctrl.h>
#include <vector>

struct TableRow {
    char name[32];
    long x1, y1, x2, y2;
};

class MyTable {
private:
    static HWND hWndTable;
    static HWND hWndListView;
    static HINSTANCE hInstTable;
    static std::vector<TableRow> rows;

    static INT_PTR CALLBACK TableDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    static void InitListView(HWND hListView);
    static void FillListView();

    MyTable();
    MyTable(const MyTable&);
    MyTable& operator=(const MyTable&);

public:
    typedef void(*HoverCallback)(int index);
    typedef void(*RemoveCallback)(int index);

    static void Toggle(HWND hWndParent, HINSTANCE hInst);
    static void Add(const char* name, long x1, long y1, long x2, long y2);
    static void Close();
    static bool IsOpen() { return hWndTable != nullptr; }

    static void SetHoverCallback(HoverCallback cb);
    static void SetRemoveCallback(RemoveCallback cb);

    static void Remove(int index);
};
