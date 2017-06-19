#include "ppc.h"
#include "m33.h"

PPC::PPC(float hfov, int _w, int _h) {

  w = _w;
  h = _h;
  a = V3(1.0f, 0.0f, 0.0f);
  b = V3(0.0f, -1.0f, 0.0f);
  float hfovRadians = hfov / 180.0f * 3.1415f;
  c = V3((float)-w/2.0f, (float)h/2.0f, -(float)w/(2.0f*tan(hfovRadians/2.0f)));
  C = V3(0.0f, 0.0f, 0.0f);

}


// projects given point, returns false if point behind head
bool PPC::Project(V3 P, V3& projP) {

  M33 abc;
  abc.SetColumn(a, 0);
  abc.SetColumn(b, 1);
  abc.SetColumn(c, 2);
  V3 q = abc.Inverted()*(P-C);

  if (q[2] <= 0.0f)
    return false;

  projP[0] = q[0] / q[2]; // u coordinate of projected point
  projP[1] = q[1] / q[2]; // v coordinate of projected point
  projP[2] = 1.0f / q[2]; // 1/w of projected point, to be used later for visibility

  return true;

}

