#pragma once


#include <math.h>
#include <iostream>

using namespace std;


class V3 {
  float xyz[3];
public:
  V3(float x, float y, float z);
  V3() {};
  // access for read/write
  float& operator[](int i);
  // addition of 2 vectors
  V3 operator+(V3 v1);
  // subtraction of 2 vectors
  V3 operator-(V3 v1);
  // dot product
  float operator*(V3 v1);
  // cross product
  V3 operator^(V3 v1);
  // multiplication by scalar
  V3 operator*(float a)const;
  // division by scalar
  V3 operator/(float a)const;
  // normalization
  void normalize();
  // length computation
  static float vectorMag(V3 v1);
  //rotation of "this" point about arbitrary axis
  V3 rotateaxis(V3 O1,V3 a,float theta);
  //rotation of "this" vector about arbitrary direction
  V3 rotatedirect(V3 a,float theta);
  //ostream
  friend ostream& operator << (ostream &os, V3 &v);
  //isream
  friend istream& operator >> (istream &is, V3 &v);
  void SetFromColor(unsigned int color);
  unsigned int GetColor();
};