#pragma once

class V3 {
  float xyz[3];
public:
  V3(float x, float y, float z);
  V3() {};
  // dot product
  float operator*(V3 v1);
  // access for read/write
  float& operator[](int i);
};