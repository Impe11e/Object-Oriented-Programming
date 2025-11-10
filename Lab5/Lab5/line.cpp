#include "line.h"

const char* LineShape::GetName() const { return "Ë³í³ÿ"; }

void LineShape::Show(HDC hdc) {
    MoveToEx(hdc, xs1, ys1, NULL);
    LineTo(hdc, xs2, ys2);
}

Shape* LineShape::Clone() const {
    LineShape* p = new LineShape();
    p->Set(xs1, ys1, xs2, ys2);
    return p;
}
