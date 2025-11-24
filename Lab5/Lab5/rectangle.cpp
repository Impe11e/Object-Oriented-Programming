#include "rectangle.h"

const char* RectShape::GetName() const { return "Прямокутник"; }

void RectShape::Show(HDC hdc) {
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, xs1, ys1, xs2, ys2);
    SelectObject(hdc, hOldBrush);
}

Shape* RectShape::Clone() const {
    RectShape* p = new RectShape();
    p->Set(xs1, ys1, xs2, ys2);
    return p;
}
