#pragma once

#include <vector>
#include "Point.h"
#include <Windows.h>

class Object3 {
public:
    static HWND Create(HINSTANCE hInstance, int x, int y);
    static void RequestReadClipboard(HWND hWnd);
};
