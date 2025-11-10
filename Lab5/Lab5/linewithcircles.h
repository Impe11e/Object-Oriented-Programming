#pragma once
#include "line.h"
#include "ellipse.h"

class LineOOShape : public LineShape, public EllipseShape
{
public:
    const char* GetName() const override;
    virtual void Show(HDC hdc) override;
    virtual Shape* Clone() const override;
};
