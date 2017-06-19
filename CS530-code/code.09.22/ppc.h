#pragma once

#include "v3.h"
#include "framebuffer.h"

class PPC {
public:
  V3 a, b, c, C;
  int w, h;
  PPC(float hfov, int _w, int _h);
  float GetF();
  V3 GetVD();
  bool Project(V3 P, V3& projP);
  void Translate(V3 translation);
  void Pan(float theta);
  void VisualizeCamera(PPC *visCam, FrameBuffer *fb, float visF);

};