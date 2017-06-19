#include "v3.h"
#include "m33.h"


V3::V3(float x, float y, float z) {

  xyz[0] = x;
  xyz[1] = y;
  xyz[2] = z;

}

float V3::operator*(V3 v1) {

  return 
    xyz[0]*v1.xyz[0] + 
    xyz[1]*v1.xyz[1] + 
    xyz[2]*v1.xyz[2];

}

float& V3::operator[](int i) {

  return xyz[i];

}

ostream& operator<<(ostream &ostr, V3 v) {

  ostr << v[0] << " " << v[1] << " " << v[2] << " ";
  return ostr;

}

unsigned int V3::GetColor() {

  unsigned int ret = 0xFF000000;

  // clamping
  V3 &v = *this;
  for (int i = 0; i < 3; i++) {
    // clamping to [0,0f, 1.0f];
    float cf = v[i];
    unsigned char cb;
    cf = (cf < 0.0f) ? 0.0f : cf;
    cf = (cf > 1.0f) ? 1.0f : cf;
    cb = (unsigned char) (cf*255.0f+0.5f);
    ((unsigned char*)&ret)[i] = cb;
  }

  return ret;

}

void V3::SetFromColor(unsigned int color) {

  float redf = (float) (((unsigned char*) (&color))[0]);
  redf = redf / 255.0f;
  float greenf = (float) (((unsigned char*) (&color))[1]);
  greenf = greenf / 255.0f;
  float bluef = (float) (((unsigned char*) (&color))[2]);
  bluef = bluef / 255.0f;

  xyz[0] = redf;
  xyz[1] = greenf;
  xyz[2] = bluef;

}

// cross product
V3 V3::operator^(V3 op1) {

  V3 ret;
  ret.xyz[0] = xyz[1]*op1.xyz[2]-xyz[2]*op1.xyz[1];
  ret.xyz[1] = xyz[2]*op1.xyz[0]-xyz[0]*op1.xyz[2];
  ret.xyz[2] = xyz[0]*op1.xyz[1]-xyz[1]*op1.xyz[0];

  return ret;

}

// vector subtraction
V3 V3::operator-(V3 op1) {

  V3 ret(
    xyz[0]-op1[0], 
    xyz[1]-op1[1], 
    xyz[2]-op1[2]);
  return ret;

}

// vector addition
V3 V3::operator+(V3 op1) {

  V3 ret(
    xyz[0]+op1[0], 
    xyz[1]+op1[1], 
    xyz[2]+op1[2]);
  return ret;

}


V3 V3::operator/(float sc) {

  V3 ret(*this);
  ret[0] /= sc;
  ret[1] /= sc;
  ret[2] /= sc;
  return ret;

}

V3 V3::operator*(float sc) {

  V3 ret(*this);
  ret[0] *= sc;
  ret[1] *= sc;
  ret[2] *= sc;
  return ret;

}

V3 V3::RotateThisVectorAboutAxis(V3 aDir, float theta) {

  return RotateThisPointAboutAxis(V3(0.0f, 0.0f, 0.0f), aDir, theta);

}

V3 V3::RotateThisPointAboutAxis(V3 Oa, V3 aDir, float theta) {

  // build aux coordinate system with axis as one of its principal axes
  V3 auxAxis;
  if (fabsf(aDir[0]) > fabsf(aDir[1])) {
    auxAxis = V3(0.0f, 1.0f, 0.0f);
  }
  else {
    auxAxis = V3(1.0f, 0.0f, 0.0f);
  }

  V3 yl = aDir;
  V3 zl = (aDir ^ auxAxis).UnitVector();
  V3 xl = (yl ^ zl).UnitVector();
  M33 lcs;
  lcs[0] = xl;
  lcs[1] = yl;
  lcs[2] = zl;

  // transform to aux coordinate system
  V3 &p = *this;
  V3 p1 = lcs*(p-Oa);

  // rotate about principal axis
  M33 roty;
  roty.SetRotationAboutY(theta);
  V3 p2 = roty * p1;

  // transform back to old world
  V3 ret = lcs.Inverted()*p2 + Oa;
  return ret;

}

V3 V3::UnitVector() {

  V3 ret = *this;
  ret = ret / Length();
  return ret;

}

float V3::Length() {

  V3 &v = *this;
  return sqrtf(v*v);

}