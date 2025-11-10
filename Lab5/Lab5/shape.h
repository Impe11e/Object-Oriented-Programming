#pragma once
#include <windows.h>

class Shape
{
protected:
	long xs1, ys1, xs2, ys2;
	
public:
	void Set(long x1, long y1, long x2, long y2);
	void Get(long& x1, long& y1, long& x2, long& y2) const;
	
	virtual void Show(HDC hdc) = 0;
	virtual void ShowTrail(HDC hdc);
	
	virtual const char* GetName() const = 0;
	virtual Shape* Clone() const = 0;
	virtual ~Shape() {}
};

class PointShape : public Shape 
{
public:
	const char* GetName() const override { return "Точка"; }
	virtual void Show(HDC hdc) override;
	virtual Shape* Clone() const override;
};

class LineShape : public virtual Shape  
{
public:
	const char* GetName() const override { return "Лінія"; }
	virtual void Show(HDC hdc) override;
	virtual Shape* Clone() const override;
};

class RectShape : public virtual Shape  
{
public:
	const char* GetName() const override { return "Прямокутник"; }
	virtual void Show(HDC hdc) override;
	virtual Shape* Clone() const override;
};

class EllipseShape : public virtual Shape 
{
public:
	const char* GetName() const override { return "Еліпс"; }
	virtual void Show(HDC hdc) override;
	virtual Shape* Clone() const override;
};

class LineOOShape : public LineShape, public EllipseShape
{
public:
	const char* GetName() const override { return "Лінія з кружечками"; }
	virtual void Show(HDC hdc) override;
	virtual Shape* Clone() const override;
};

class CubeShape : public LineShape, public RectShape
{
public:
	const char* GetName() const override { return "Куб"; }
	virtual void Show(HDC hdc) override;
	virtual Shape* Clone() const override;
};