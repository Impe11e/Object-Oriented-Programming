Shape* PointShape::Clone() const { return new PointShape(*this); }
Shape* LineShape::Clone() const { return new LineShape(*this); }
Shape* RectShape::Clone() const { return new RectShape(*this); }
Shape* EllipseShape::Clone() const { return new EllipseShape(*this); }
Shape* LineOOShape::Clone() const { return new LineOOShape(*this); }
Shape* CubeShape::Clone() const { return new CubeShape(*this); }