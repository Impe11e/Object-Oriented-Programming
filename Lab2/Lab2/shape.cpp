#include "shape.h"

void PointShape::Show(HDC hdc) {
	SetPixel(hdc, xs1, ys1, RGB(0, 0, 0));
}

void LineShape::Show(HDC hdc)
{
    MoveToEx(hdc, xs1, ys1, NULL);
    LineTo(hdc, xs2, ys2);
}

void RectShape::Show(HDC hdc)
{
    MoveToEx(hdc, xs1, ys1, NULL);
    LineTo(hdc, xs2, ys1);
    LineTo(hdc, xs2, ys2);
    LineTo(hdc, xs1, ys2);
    LineTo(hdc, xs1, ys1);
}

void EllipseShape::Show(HDC hdc)
{
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 255)); // рожевий
    HBRUSH hBrushOld = (HBRUSH)SelectObject(hdc, hBrush);
    
    Ellipse(hdc, xs1, ys1, xs2, ys2);
    
    SelectObject(hdc, hBrushOld);
    DeleteObject(hBrush);
}