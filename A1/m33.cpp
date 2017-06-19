#include "m33.h"
#include "v3.h"

// access to rows for reading and writing
V3& M33::operator[](int i) {

  return rows[i];

}

// get column 
V3 M33::GetColumn(int ci){
	V3 col;
	col[0]=rows[ci][0];
	col[1]=rows[ci][1];
	col[2]=rows[ci][2];
	return col;
}

// set column
void M33::SetColumn(V3 newColumn, int ci) {

  rows[0][ci] = newColumn[0];
  rows[1][ci] = newColumn[1];
  rows[2][ci] = newColumn[2];

}
// multiplication between matrix and vector
V3 M33::operator*(V3 v) {

  V3 ret;
  ret[0] = rows[0]*v;
  ret[1] = rows[1]*v;
  ret[2] = rows[2]*v;
  return ret;

}
// multiplication between matrix and matrix
V3 M33::operator*(M33 m){

	V3 ret;
	ret[0] = rows[0]*(m.GetColumn(0));
	ret[1] = rows[1]*(m.GetColumn(1));
	ret[2] = rows[2]*(m.GetColumn(2));
	return ret;
}

// matrix inverse
M33 M33::Inverted() {

  M33 ret;
  V3 a = GetColumn(0), b = GetColumn(1), c = GetColumn(2);
  V3 _a = b ^ c; _a = _a / (a * _a);
  V3 _b = c ^ a; _b = _b / (b * _b);
  V3 _c = a ^ b; _c = _c / (c * _c);
  ret[0] = _a;
  ret[1] = _b;
  ret[2] = _c;

  return ret;

}
// transposition
M33 M33::Transpose(){

	M33 ret;
	float t;
	t = ret[0][0];
	ret[0][0]=ret[2][2];
	ret[2][2]=t;

	t= ret[0][1];
	ret[0][1]=ret[1][0];
	ret[1][0]=t;

	t= ret[1][0];
	ret[1][0]=ret[0][1];
	ret[0][1]=t;

	return ret;

}

void M33::SetRotationAboutY(float theta) {

  float thetaRadians = theta / 180.0f *3.14159f;
  rows[0] = V3(cos(thetaRadians), 0.0f, sin(thetaRadians));
  rows[1] = V3(0.0f, 1.0f, 0.0f);
  rows[2] = V3(-sin(thetaRadians), 0.0f, cos(thetaRadians));

}

ostream& operator<<(ostream &os, M33 m) {

  return os
    << m[0] << endl
    << m[1] << endl
    << m[2] << endl;

}

istream& operator>>(istream &is, M33 m){
	cout<< "please input the matrix:"<<endl;
	is >> m[0];
	is >> m[1];
	is >> m[2];
	return is;

}


