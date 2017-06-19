#pragma once

#include "v3.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
class PPC {
public:
  V3 a, b, c, C;
  int w, h;
  PPC(float hfov, int _w, int _h);
  bool Project(V3 P, V3& projP);
  void Transtoright(int step);
  void Transtodown(int step);
  void Transtoback(int step);
  void pan(float theta);
  void tilt(float theta);
  void roll(float theta);
  float GetF();
  void zoom(float zoom);
  void SetByInterpolation(PPC *ppc0,PPC *ppc1,int i,int n);
  void saveText(char *fname);
  void loadText(char *fname);
};