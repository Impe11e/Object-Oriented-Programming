#pragma once

#include <vector>
#include "Point.h"
#include <Windows.h>

class Object2 {
public:
    static HWND Create(HINSTANCE hInstance, int x, int y);
    static void ShowPoints(HWND hWnd, const std::vector<PointI>& pts);
};
