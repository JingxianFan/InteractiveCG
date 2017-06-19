#include "TMesh.h"

// constructs a tetrahedron with the given vertices
TMesh::TMesh(V3 *_verts, V3 *_cols) {

  vertsN = 4;
  verts = new V3[vertsN];
  cols = new V3[vertsN];
  for (int i = 0; i < 4; i++) {
    verts[i] = _verts[i];
    cols[i] = _cols[i];
  }

  trisN = 4;
  tris = new unsigned int[3*trisN];

  int tri = 0;
  tris[3*tri+0] = 0;
  tris[3*tri+1] = 1;
  tris[3*tri+2] = 2;

  tri++;
  tris[3*tri+0] = 0;
  tris[3*tri+1] = 2;
  tris[3*tri+2] = 3;

  tri++;
  tris[3*tri+0] = 0;
  tris[3*tri+1] = 3;
  tris[3*tri+2] = 1;

  tri++;
  tris[3*tri+0] = 1;
  tris[3*tri+1] = 3;
  tris[3*tri+2] = 2;

}
