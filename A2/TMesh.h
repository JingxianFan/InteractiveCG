#pragma once

#include "v3.h"
#include "aabb.h"
#include "ppc.h"

class TMesh {
public:
  V3 *verts, *cols;
  int vertsN;
  unsigned int *tris;
  int trisN; // number of triangle (not number of indices in array)
  TMesh(V3 *_verts, V3 *_cols); // constructs a tetrahedron with the given vertices
  TMesh() : verts(0), cols(0), vertsN(0), tris(0), trisN(0) {};
  void LoadBin(char *fname);
  // returns center of mass of vertices
  V3 GetCenter();
  // computes a bounding box of the vertices
  AABB ComputeAABB();
  // translation of vertices
  void vertLeftToRight(V3 verts,PPC ppc,int step);
  void vertUpToDown(V3 verts,PPC ppc,int step);
  void vertForwardToBack(V3 verts,PPC ppc,int step);
  // scaling of vertices
  void vertScaling(TMesh *tm,float scale);
  //place cenroid and scaling to aabb size
  void placeTm(TMesh *tm,V3 centroid,AABB box);
   // rotate about axis
  void RotateAboutAxis(V3 aO, V3 adir, float theta);
};