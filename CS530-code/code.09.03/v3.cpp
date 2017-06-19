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