#include "TMesh.h"
#include <fstream>

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

void TMesh::LoadBin(char *fname) {

  ifstream ifs(fname, ios::binary);
  if (ifs.fail()) {
    cerr << "INFO: cannot open file: " << fname << endl;
    return;
  }

  ifs.read((char*)&vertsN, sizeof(int));
  char yn;
  ifs.read(&yn, 1); // always xyz
  if (yn != 'y') {
    cerr << "INTERNAL ERROR: there should always be vertex xyz data" << endl;
    return;
  }
  if (verts)
    delete verts;
  verts = new V3[vertsN];

  ifs.read(&yn, 1); // cols 3 floats
  if (cols)
    delete cols;
  cols = 0;
  if (yn == 'y') {
    cols = new V3[vertsN];
  }

  ifs.read(&yn, 1); // normals 3 floats
  V3 *normals = 0; // TMesh doesn't store normals for now
  if (normals)
    delete normals;
  normals = 0;
  if (yn == 'y') {
    normals = new V3[vertsN];
  }

  ifs.read(&yn, 1); // texture coordinates 2 floats
  float *tcs = 0; // don't have texture coordinates for now
  if (tcs)
    delete tcs;
  tcs = 0;
  if (yn == 'y') {
    tcs = new float[vertsN*2];
  }

  ifs.read((char*)verts, vertsN*3*sizeof(float)); // load verts

  if (cols) {
    ifs.read((char*)cols, vertsN*3*sizeof(float)); // load cols
  }

  if (normals)
    ifs.read((char*)normals, vertsN*3*sizeof(float)); // load normals

  if (tcs)
    ifs.read((char*)tcs, vertsN*2*sizeof(float)); // load texture coordinates

  ifs.read((char*)&trisN, sizeof(int));
  if (tris)
    delete tris;
  tris = new unsigned int[trisN*3];
  ifs.read((char*)tris, trisN*3*sizeof(unsigned int)); // read tiangles

  ifs.close();

  cerr << "INFO: loaded " << vertsN << " verts, " << trisN << " tris from " << endl << "      " << fname << endl;
  cerr << "      xyz " << ((cols) ? "rgb " : "") << ((normals) ? "nxnynz " : "") << ((tcs) ? "tcstct " : "") << endl;

  delete []tcs;
  delete []normals;

}

V3 TMesh::GetCenter() {

  V3 ret(0.0f, 0.0f, 0.0f);
  for (int vi = 0; vi < vertsN; vi++) {
    ret = ret + verts[vi];
  }
  ret = ret / (float) vertsN;
  return ret;

}

// computes a bounding box of the vertices
AABB TMesh::ComputeAABB() {

  AABB ret(verts[0]);
  for (int vi = 0; vi < vertsN; vi++) {
    ret.AddPoint(verts[vi]);
  }

  return ret;

}
