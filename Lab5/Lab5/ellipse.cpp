#include "ellipse.h"

const char* EllipseShape::GetName() const { return "Ελ³ορ"; }

void EllipseShape::Show(HDC hdc) {
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 165, 0));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    Ellipse(hdc, xs1, ys1, xs2, ys2);

    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
}

Shape* EllipseShape::Clone() const {
    EllipseShape* p = new EllipseShape();
    p->Set(xs1, ys1, xs2, ys2);
    return p;
}
