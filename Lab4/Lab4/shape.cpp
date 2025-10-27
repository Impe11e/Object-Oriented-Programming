#include "shape.h"

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
	long width = abs(xs2 - xs1);
	long height = abs(ys2 - ys1);
	long depth = min(width, height) / 3;
	
	long origX1 = xs1;
	long origY1 = ys1;
	long origX2 = xs2;
	long origY2 = ys2;
	
	long frontX1 = xs1;
	long frontY1 = ys1 + depth;
	long frontX2 = xs2 - depth;
	long frontY2 = ys2;
	
	long backX1 = xs1 + depth;
	long backY1 = ys1;
	long backX2 = xs2;
	long backY2 = ys2 - depth;
	
	Shape::Set(backX1, backY1, backX2, backY2);
	RectShape::Show(hdc);
	
	Shape::Set(frontX1, frontY1, frontX2, frontY2);
	RectShape::Show(hdc);
	
	Shape::Set(frontX1, frontY1, backX1, backY1);
	LineShape::Show(hdc);
	
	Shape::Set(frontX2, frontY1, backX2, backY1);
	LineShape::Show(hdc);
	
	Shape::Set(frontX2, frontY2, backX2, backY2);
	LineShape::Show(hdc);
	
	Shape::Set(frontX1, frontY2, backX1, backY2);
	LineShape::Show(hdc);
	
	Shape::Set(origX1, origY1, origX2, origY2);
}