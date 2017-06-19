#pragma once

#include "v3.h"

class M33 {
  V3 rows[3];
public:
  M33() {};
  // access to rows for reading and writing
  V3& operator[](int i);
};