#include "ppc.h"
#include "m33.h"

PPC::PPC(float hfov, int _w, int _h) {

  w = _w;
  h = _h;
  a = V3(1.0f, 0.0f, 0.0f);
  b = V3(0.0f, -1.0f, 0.0f);
  float hfovRadians = hfov / 180.0f * 3.1415f;
  c = V3((float)-w/2.0f, (float)h/2.0f, -(float)w/(2.0f*tan(hfovRadians/2.0f)));
  C = V3(0.0f, 0.0f, 0.0f);

}


// projects given point, returns false if point behind head
bool PPC::Project(V3 P, V3& projP) {

  // flag that projection did not succeed
  projP[0] = FLT_MAX;

  M33 abc;
  abc.SetColumn(a, 0);
  abc.SetColumn(b, 1);
  abc.SetColumn(c, 2);
  V3 q = abc.Inverted()*(P-C);

  if (q[2] <= 0.0f)
    return false;

  projP[0] = q[0] / q[2]; // u coordinate of projected point
  projP[1] = q[1] / q[2]; // v coordinate of projected point
  projP[2] = 1.0f / q[2]; // 1/w of projected point, to be used later for visibility

  return true;

}

void PPC::Translate(V3 translation) {

  C = C + translation;

}

void PPC::Pan(float theta) {

  V3 aDir = (b*-1.0f).UnitVector();

  a = a.RotateThisVectorAboutAxis(aDir, theta);
//  b = b.RotateThisVectorAboutAxis(aDir, theta);
  c = c.RotateThisVectorAboutAxis(aDir, theta);

}

void PPC::Roll(float theta) {

  V3 aDir = GetVD();

  a = a.RotateThisVectorAboutAxis(aDir, theta);
  b = b.RotateThisVectorAboutAxis(aDir, theta);
  c = c.RotateThisVectorAboutAxis(aDir, theta);

}

void PPC::Tilt(float theta) {

  V3 aDir = a.UnitVector();

  b = b.RotateThisVectorAboutAxis(aDir, theta);
  c = c.RotateThisVectorAboutAxis(aDir, theta);

}



// draw camera frustum in wireframe, adapt focal length to visF
void PPC::VisualizeCamera(PPC *visCam, FrameBuffer *fb, float visF) {

  float scf = visF / GetF();
  V3 c0(1.0f, 1.0f, 0.0f), c1(0.0f, 0.0f, 0.0f);
  fb->Draw3DSegment(C+c*scf, c1, 
    C+(c+a*(float)w)*scf, c1, visCam);
  fb->Draw3DSegment(C+(c+a*(float)w)*scf, c1, 
    C+(c+a*(float)w+b*(float)h)*scf, c1, visCam);
  fb->Draw3DSegment(C+(c+a*(float)w+b*(float)h)*scf, c1, 
    C+(c+b*(float)h)*scf, c1, visCam);
  fb->Draw3DSegment(C+(c+b*(float)h)*scf, c1, 
    C+c*scf, c1, visCam);

  fb->Draw3DSegment(C, c0, C+c*scf, c1, visCam);
  fb->Draw3DSegment(C, c0, C+(c+a*(float)w)*scf, c1, visCam);
  fb->Draw3DSegment(C, c0, C+(c+a*(float)w+b*(float)h)*scf, c1, visCam);
  fb->Draw3DSegment(C, c0, C+(c+b*(float)h)*scf, c1, visCam);

}

float PPC::GetF() {

  return GetVD()*c;

}

V3 PPC::GetVD() {

  return (a^b).UnitVector();

}

void PPC::PositionAndOrient(V3 newC, V3 lookAtPoint, V3 vInVPlane) {

  // we need to compute the camera elements for the new position and orientation
  V3 newa, newb, newc; // newC is given

  V3 newVD = (lookAtPoint - newC).UnitVector();
  newa = (newVD ^ vInVPlane).UnitVector()*a.Length();
  newb = (newVD ^ newa).UnitVector()*b.Length();
  newc = newVD*GetF() -newa*(float)w/2.0f -newb*(float)h/2.0f;

  // commit new values
  C = newC;
  a = newa;
  b = newb;
  c = newc;

}


void PPC::SetInterpolated(PPC *ppc0, PPC *ppc1, int stepi, int stepsN) {

  cerr << "INFO: not yet implemented" << endl;

  // compute newC and newVD by interpolating between C0 and C1, and vd0 and vd1
  // compute vInVPlane by interpolating between b0 and b1, and multiplying by -1
  // call position and orient function we have developed in class

}

V3 PPC::Unproject(V3& projP) {

  V3 ret = C + (a*projP[0] + b*projP[1] + c)/projP[2];

  return ret;

}
