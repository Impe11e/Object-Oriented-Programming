#pragma once
#include "line.h"
#include "rectangle.h"

class CubeShape : public LineShape, public RectShape
{
public:
    const char* GetName() const override;
    virtual void Show(HDC hdc) override;
    virtual Shape* Clone() const override;
};
