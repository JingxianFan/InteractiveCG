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
  // access for read/write
  float& operator[](int i);
  friend ostream& operator<<(ostream &ostr, V3 v);
  void SetFromColor(unsigned int color);
  unsigned int GetColor();
};