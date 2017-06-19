#include "v3.h"
#include "m33.h"


V3::V3(float x, float y, float z) {

  xyz[0] = x;
  xyz[1] = y;
  xyz[2] = z;

}
float& V3::operator[](int i) {

  return xyz[i];

}
V3 V3::operator+(V3 v1)const{
  return
	V3(xyz[0]+v1[0],xyz[1]+v1[1],xyz[2]+v1[2]);
}

V3 V3::operator-(V3 v1)const{
  return
	V3(xyz[0]-v1[0],xyz[1]-v1[1],xyz[2]-v1[2]);
}
float V3::operator*(V3 v1) {

  return 
    xyz[0]*v1.xyz[0] + 
    xyz[1]*v1.xyz[1] + 
    xyz[2]*v1.xyz[2];

}

V3 V3::operator^(V3 v1){
  return
	  V3(xyz[1]*v1[2]-xyz[2]*v1[1],-xyz[0]*v1[2]+xyz[2]*v1[0],xyz[0]*v1[1]-xyz[1]*v1[0]);
}
V3 V3::operator*(float a)const{
  return
	  V3(a*xyz[0],a*xyz[1],a*xyz[2]);
}
V3 V3::operator/(float a)const{
  return
	  V3(xyz[0]/a,xyz[1]/a,xyz[2]/a);
}
void V3::normalize(){
  float magsq = xyz[0]*xyz[0]+xyz[1]*xyz[1]+xyz[2]*xyz[2];
  if (magsq > 0.0f){
	float oneOverMag = 1.0f/sqrt(magsq);
	xyz[0]*=oneOverMag;
	xyz[1]*=oneOverMag;
	xyz[2]*=oneOverMag;
  }
}
float V3::vectorMag(V3 v1){
  return sqrt(v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2]);
}



V3 V3::rotateaxis(V3 O1,V3 A,float theta){
  V3 x(1,0,0),z(0,0,1);
  V3 _y=A; //_y is the axis that point rotate about
  _y.normalize();
  V3 _x;
  if (abs((x*_y)) < (abs((z*_y)))){
	_x=x^_y;
  }
  else{
	_x=z^_y;
  }
  _x.normalize();
  V3 _z = _y^_x;//step1,_x,_y,_z is the new coordinates' axes.
  _z.normalize();

  V3 p0=*this-O1;//translate
  V3 p1(p0*_x,p0*_y,p0*_z);//Step2,p1 is "this" point in the new coordinates.
  M33 rot;
  rot.SetRotationAboutY(theta);
  V3 p2 =(rot)*(p1);//Step3,rotate about _y.
  M33 m;
  m[0]=_x;
  m[1]=_y;
  m[2]=_z;
  V3 p3 = m.Inverted()*p2+O1;//Step4,transform back to older coordinaties.
  return p3;
}


V3 V3::rotatedirect(V3 a,float theta){
  V3 x(1,0,0),z(0,0,1);
  V3 _y=a; //_y is the axis that point rotate about
  _y.normalize();
  V3 _x;
  if (abs((x*_y)) < (abs((z*_y)))){
	_x=x^_y;
  }
  else{
	_x=z^_y;
  }
  _x.normalize();
  V3 _z = _y^_x;//step1,_x,_y,_z is the new coordinates' axes.
  _z.normalize();

  V3 p0=*this;
  V3 p1(p0*_x,p0*_y,p0*_z);//Step2,p1 is "this" point in the new coordinates.
  M33 rot;
  rot.SetRotationAboutY(theta);
  V3 p2 =(rot)*(p1);//Step3,rotate about _y.
  M33 m;
  m[0]=_x;
  m[1]=_y;
  m[2]=_z;
  V3 p3 = m.Inverted()*p2;//Step4,transform back to older coordinaties.
  return p3;
}

ostream &operator << (ostream &os, V3 &v){
	os<<"("<<v[0]<<","<<v[1]<<","<<v[2]<<")"<<endl;
	return os;
}

istream &operator >> (istream &is, V3 &v){
	cout<<"please input coordinates:"<<endl;
	is >> v[0];
	is >> v[1];
	is >> v[2];
	return is;
}

unsigned int V3::GetColor() {

  unsigned int ret = 0xFF000000;

  // clamping
  V3 &v = *this;
  for (int i = 0; i < 3; i++) {
    // clamping to [0,0f, 1.0f];
    float cf = v[i];
    unsigned char cb;
    cf = (cf < 0.0f) ? 0.0f : cf;
    cf = (cf > 1.0f) ? 1.0f : cf;
    cb = (unsigned char) (cf*255.0f+0.5f);
    ((unsigned char*)&ret)[i] = cb;
  }

  return ret;

}

void V3::SetFromColor(unsigned int color) {

  float redf = (float) (((unsigned char*) (&color))[0]);
  redf = redf / 255.0f;
  float greenf = (float) (((unsigned char*) (&color))[1]);
  greenf = greenf / 255.0f;
  float bluef = (float) (((unsigned char*) (&color))[2]);
  bluef = bluef / 255.0f;

  xyz[0] = redf;
  xyz[1] = greenf;
  xyz[2] = bluef;

}
