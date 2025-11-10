#pragma once
#include "framework.h"
#include "resource.h"
#include <commctrl.h>

class Toolbar {
private:
    HWND hToolbar;
    HWND hParentWnd;
    HINSTANCE hInstance;

public:
    Toolbar();
    ~Toolbar();

    bool Create(HWND hParent, HINSTANCE hInst);

    HWND GetHandle() const { return hToolbar; }

    void AutoSize();

    bool IsCreated() const { return hToolbar != nullptr; }

private:
    void AddButtons();

    void SetupImageList();
};
