#include "v3.h"


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