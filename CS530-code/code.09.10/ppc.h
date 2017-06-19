#pragma once

#include "v3.h"

class PPC {
public:
  V3 a, b, c, C;
  int w, h;
  PPC(float hfov, int _w, int _h);
  bool Project(V3 P, V3& projP);
};