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