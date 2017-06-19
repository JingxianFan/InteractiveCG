#include "m33.h"

// access to rows for reading and writing
V3& M33::operator[](int i) {

  return rows[i];

}

ostream& operator<<(ostream &ostr, M33 m) {

  return ostr 
    << m[0] << endl
    << m[1] << endl
    << m[2] << endl;

}

// multiplication between matrix and vector
V3 M33::operator*(V3 v) {

  V3 ret;
  ret[0] = rows[0]*v;
  ret[1] = rows[1]*v;
  ret[2] = rows[2]*v;
  return ret;

}

void M33::SetColumn(V3 newColumn, int ci) {

  rows[0][ci] = newColumn[0];
  rows[1][ci] = newColumn[1];
  rows[2][ci] = newColumn[2];

}

V3 M33::GetColumn(int ci) {

  V3 ret;
  ret[0] = rows[0][ci];
  ret[1] = rows[1][ci];
  ret[2] = rows[2][ci];
  return ret;
}

void M33::SetRotationAboutY(float theta) {

  float thetaRadians = theta / 180.0f *3.14159f;
  rows[0] = V3(cos(thetaRadians), 0.0f, sin(thetaRadians));
  rows[1] = V3(0.0f, 1.0f, 0.0f);
  rows[2] = V3(-sin(thetaRadians), 0.0f, cos(thetaRadians));

}

M33 M33::Inverted() {

  M33 ret;
  V3 a = GetColumn(0), b = GetColumn(1), c = GetColumn(2);
  V3 _a = b ^ c; _a = _a / (a * _a);
  V3 _b = c ^ a; _b = _b / (b * _b);
  V3 _c = a ^ b; _c = _c / (c * _c);
  ret[0] = _a;
  ret[1] = _b;
  ret[2] = _c;

  return ret;

}
