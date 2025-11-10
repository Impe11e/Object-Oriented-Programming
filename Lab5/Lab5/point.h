#pragma once
#include "shape.h"

class PointShape : public Shape
{
public:
    const char* GetName() const override;
    virtual void Show(HDC hdc) override;
    virtual Shape* Clone() const override;
};
