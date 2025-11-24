#pragma once

#include "resource.h"
#include <vector>
#include <Windows.h>
#include <string>
#include <random>

struct PointInt { int x; int y; };

class DataGenerator {
public:
    DataGenerator();
    ~DataGenerator();

    void GeneratePoints(int nPoints, int xMin, int xMax, int yMin, int yMax);
    bool PutTextToClipboardW(HWND hWnd, const std::wstring &text);
    void OnPaint(HWND hWnd, HDC hdc);

    const std::vector<PointInt>& GetPoints() const { return points; }

private:
    std::vector<PointInt> points;
    std::mt19937 rng;
};
