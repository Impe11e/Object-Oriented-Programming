#include "shape.h"
#include <windows.h>

void Shape::Set(long x1, long y1, long x2, long y2)
{
    xs1 = x1;
    ys1 = y1;
    xs2 = x2;
    ys2 = y2;
}

void Shape::Get(long& x1, long& y1, long& x2, long& y2) const
{
    x1 = xs1;
    y1 = ys1;
    x2 = xs2;
    y2 = ys2;
}

void Shape::ShowTrail(HDC hdc)
{
    HPEN hDashPen = CreatePen(PS_DASH, 1, RGB(255, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hDashPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    
    Show(hdc);
    
    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hDashPen);
}
