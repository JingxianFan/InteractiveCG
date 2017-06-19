#pragma once

#include "v3.h"

class TMesh {
public:
  V3 *verts, *cols;
  int vertsN;
  unsigned int *tris;
  int trisN; // number of triangle (not number of indices in array)
  TMesh(V3 *_verts, V3 *_cols); // constructs a tetrahedron with the given vertices
};