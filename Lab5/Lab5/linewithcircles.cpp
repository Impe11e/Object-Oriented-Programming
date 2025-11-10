#include "linewithcircles.h"
#include <algorithm>

namespace ShapeConstants {
    constexpr int LINEOO_RADIUS = 5;
}

const char* LineOOShape::GetName() const { return "Лінія з кружечками"; }

void LineOOShape::Show(HDC hdc) {
    const int RADIUS = ShapeConstants::LINEOO_RADIUS;

    long lineX1 = xs1;
    long lineY1 = ys1;
    long lineX2 = xs2;
    long lineY2 = ys2;

    LineShape::Show(hdc);

    HBRUSH hCircleBrush = CreateSolidBrush(RGB(255, 165, 0));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hCircleBrush);

    Shape::Set(lineX1 - RADIUS, lineY1 - RADIUS, lineX1 + RADIUS, lineY1 + RADIUS);
    EllipseShape::Show(hdc);

    Shape::Set(lineX2 - RADIUS, lineY2 - RADIUS, lineX2 + RADIUS, lineY2 + RADIUS);
    EllipseShape::Show(hdc);

    Shape::Set(lineX1, lineY1, lineX2, lineY2);

    SelectObject(hdc, hOldBrush);
    DeleteObject(hCircleBrush);
}

Shape* LineOOShape::Clone() const {
    LineOOShape* p = new LineOOShape();
    p->Set(xs1, ys1, xs2, ys2);
    return p;
}
