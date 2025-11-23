#pragma once

#include <Windows.h>
#include "Point.h"

class InputDialog {
public:
    InputDialog(HINSTANCE hInst_, HWND owner);
    ~InputDialog();
    HWND Create();

    struct Impl; // forward declaration made public so implementation file can define it

private:
    Impl* pImpl;
};
