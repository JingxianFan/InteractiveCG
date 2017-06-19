#pragma once

#include "v3.h"

class AABB {
public:
  V3 corners[2];
  AABB(V3 firstPoint);
  void AddPoint(V3 newPoint);
  bool ClipWithFrame(float left, float top, float right, float bottom);
  void SetPixelRectangle(int& left, int& right, int& top, int& bottom);
};