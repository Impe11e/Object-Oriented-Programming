#include "point.h"

const char* PointShape::GetName() const { return "Точка"; }

void PointShape::Show(HDC hdc) {
    SetPixel(hdc, xs1, ys1, RGB(0, 0, 0));
}

Shape* PointShape::Clone() const {
    PointShape* p = new PointShape();
    p->Set(xs1, ys1, xs2, ys2);
    return p;
}
