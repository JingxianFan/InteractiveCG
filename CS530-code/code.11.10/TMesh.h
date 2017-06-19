#pragma once

#include "v3.h"
#include "aabb.h"

class PPC;
class FrameBuffer;

class TMesh {
public:
  V3 *verts, *cols, *projVerts, *normals;
  FrameBuffer *texture;
  bool enabled;
  int vertsN;
  unsigned int *tris;
  int trisN; // number of triangle (not number of indices in array)
  TMesh(V3 *_verts, V3 *_cols); // constructs a tetrahedron with the given vertices
  TMesh() : verts(0), cols(0), normals(0), 
    vertsN(0), tris(0), trisN(0), projVerts(0), enabled(false),
    texture(0) {};
  void SetAsRectangle(V3 *corners);
  void LoadBin(char *fname);
  // returns center of mass of vertices
  V3 GetCenter();
  // computes a bounding box of the vertices
  AABB ComputeAABB();
  // rotate about axis
  void RotateAboutAxis(V3 aO, V3 adir, float theta);
  void ProjectVertices(PPC *ppc);
  void Translate(V3 transv);
  void AllocateData();
  void RenderHW();
};