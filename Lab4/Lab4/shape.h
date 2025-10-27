#pragma once
#include <windows.h>

class Shape
{
protected:
	long xs1, ys1, xs2, ys2;
	
public:
	void Set(long x1, long y1, long x2, long y2);
	virtual void Show(HDC hdc) = 0;
	virtual Shape* Clone() const = 0;
	virtual ~Shape() {}
};

class PointShape : public Shape 
{
public:
	virtual void Show(HDC hdc) override;
	virtual Shape* Clone() const override;
};

class LineShape : public virtual Shape  
{
public:
	virtual void Show(HDC hdc) override;
	virtual Shape* Clone() const override;
};

class RectShape : public virtual Shape  
{
public:
	virtual void Show(HDC hdc) override;
	virtual Shape* Clone() const override;
};

class EllipseShape : public virtual Shape 
{
public:
	virtual void Show(HDC hdc) override;
	virtual Shape* Clone() const override;
};

class LineOOShape : public LineShape, public EllipseShape
{
public:
	virtual void Show(HDC hdc) override;
	virtual Shape* Clone() const override;
};

class CubeShape : public LineShape, public RectShape
{
public:
	virtual void Show(HDC hdc) override;
	virtual Shape* Clone() const override;
};