#include "shape.h"
#include <algorithm>

using std::min;
using std::max;

void Shape::Set(long x1, long y1, long x2, long y2)
{
	xs1 = x1;
	ys1 = y1;
	xs2 = x2;
	ys2 = y2;
}

void PointShape::Show(HDC hdc) 
{
	SetPixel(hdc, xs1, ys1, RGB(0, 0, 0));
}

void LineShape::Show(HDC hdc)
{
	MoveToEx(hdc, xs1, ys1, NULL);
	LineTo(hdc, xs2, ys2);
}

void RectShape::Show(HDC hdc)
{
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
	Rectangle(hdc, xs1, ys1, xs2, ys2);
	SelectObject(hdc, hOldBrush);
}

void EllipseShape::Show(HDC hdc)
{
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 165, 0));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	
	Ellipse(hdc, xs1, ys1, xs2, ys2);
	
	SelectObject(hdc, hOldBrush);
	DeleteObject(hBrush);
}

void LineOOShape::Show(HDC hdc)
{
	const int RADIUS = 5;
	
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

void CubeShape::Show(HDC hdc)
{
	long left = min(xs1, xs2);
	long right = max(xs1, xs2);
	long top = min(ys1, ys2);
	long bottom = max(ys1, ys2);

	long width = right - left;
	long height = bottom - top;
	if (width <= 0 || height <= 0) return;

	long depth = max(4L, min(width, height) / 4);

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

Shape* PointShape::Clone() const {
	PointShape* p = new PointShape();
	p->Set(xs1, ys1, xs2, ys2);
	return p;
}

Shape* LineShape::Clone() const {
	LineShape* p = new LineShape();
	p->Set(xs1, ys1, xs2, ys2);
	return p;
}

Shape* RectShape::Clone() const {
	RectShape* p = new RectShape();
	p->Set(xs1, ys1, xs2, ys2);
	return p;
}

Shape* EllipseShape::Clone() const {
	EllipseShape* p = new EllipseShape();
	p->Set(xs1, ys1, xs2, ys2);
	return p;
}

Shape* LineOOShape::Clone() const {
	LineOOShape* p = new LineOOShape();
	p->Set(xs1, ys1, xs2, ys2);
	return p;
}

Shape* CubeShape::Clone() const {
	CubeShape* p = new CubeShape();
	p->Set(xs1, ys1, xs2, ys2);
	return p;
}
