#include "m33.h"

// access to rows for reading and writing
V3& M33::operator[](int i) {

  return rows[i];

}

