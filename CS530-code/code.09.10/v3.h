#pragma once

#include <iostream>

using namespace std;

class V3 {
  float xyz[3];
public:
  V3(float x, float y, float z);
  V3() {};
  // dot product
  float operator*(V3 v1);
  // cross product
  V3 operator^(V3 op1);
  // vector subtraction
  V3 operator-(V3 op1);
  // vector addition
  V3 operator+(V3 op1);
  // access for read/write
  float& operator[](int i);
  // division by scalar
  V3 operator/(float sc);
  // multiplication by scalar
  V3 operator*(float sc);
  friend ostream& operator<<(ostream &ostr, V3 v);
  void SetFromColor(unsigned int color);
  unsigned int GetColor();
};