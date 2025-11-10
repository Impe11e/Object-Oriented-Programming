#include "cube.h"

const char* CubeShape::GetName() const { return " Û·"; }

void CubeShape::Show(HDC hdc) {
    long left = (xs1 < xs2) ? xs1 : xs2;
    long right = (xs1 > xs2) ? xs1 : xs2;
    long top = (ys1 < ys2) ? ys1 : ys2;
    long bottom = (ys1 > ys2) ? ys1 : ys2;

    long width = right - left;
    long height = bottom - top;
    if (width <= 0 || height <= 0) return;

    long minWH = (width < height) ? width : height;
    long depth = (4 > (minWH / 4)) ? 4 : (minWH / 4);

    long frontX1 = left;
    long frontY1 = top + depth;
    long frontX2 = right - depth;
    long frontY2 = bottom;

    long backX1 = left + depth;
    long backY1 = top;
    long backX2 = right;
    long backY2 = bottom - depth;

    long ox1 = xs1, oy1 = ys1, ox2 = xs2, oy2 = ys2;

    Shape::Set(backX1, backY1, backX2, backY2);
    RectShape::Show(hdc);

    Shape::Set(frontX1, frontY1, frontX2, frontY2);
    RectShape::Show(hdc);

    Shape::Set(ox1, oy1, ox2, oy2);

    MoveToEx(hdc, (int)frontX1, (int)frontY1, NULL); LineTo(hdc, (int)backX1, (int)backY1);
    MoveToEx(hdc, (int)frontX2, (int)frontY1, NULL); LineTo(hdc, (int)backX2, (int)backY1);
    MoveToEx(hdc, (int)frontX2, (int)frontY2, NULL); LineTo(hdc, (int)backX2, (int)backY2);
    MoveToEx(hdc, (int)frontX1, (int)frontY2, NULL); LineTo(hdc, (int)backX1, (int)backY2);
}

Shape* CubeShape::Clone() const {
    CubeShape* p = new CubeShape();
    p->Set(xs1, ys1, xs2, ys2);
    return p;
}
