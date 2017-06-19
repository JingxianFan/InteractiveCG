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
  V3 Unproject(V3& projP);
  void Translate(V3 translation);
  void Pan(float theta);
  void Tilt(float theta);
  void Roll(float theta);
  void VisualizeCamera(PPC *visCam, FrameBuffer *fb, float visF);
  void PositionAndOrient(V3 newC, V3 lookAtPoint, V3 vInVPlane);
  void SetInterpolated(PPC *ppc0, PPC *ppc1, int stepi, int stepsN);
};