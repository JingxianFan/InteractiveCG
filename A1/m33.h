#pragma once

#include "v3.h"

class M33 {
  V3 rows[3];
public:
  M33() {};
  // access to rows for reading and writing
  V3& operator[](int i);
  // get column
  V3 GetColumn(int ci);
  // set column
  void SetColumn(V3 newColumn,int ci);
  // multiplication with 3D vector
  V3 operator*(V3 v);
  // multiplication with another matrix
  V3 operator*(M33 m);
   // inversion
  M33 Inverted();
  // transposition
  M33 Transpose();
  // rotation
  void SetRotationAboutY(float theta);
  //output to ostream,input to istream
  friend ostream& operator<<(ostream &os,M33 m);
  friend istream& operator>>(istream &is,M33 m);
};
