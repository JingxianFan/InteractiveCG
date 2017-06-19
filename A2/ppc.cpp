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

// translation
void PPC::Transtoright(int step){
	C=C+a*(float)step;
}
void PPC::Transtodown(int step){
	C=C-b*(float)step;
}
void PPC::Transtoback(int step){
	C=C+(a^b)*(float)step;
}
// pan
void PPC::pan(float theta){
	a.rotateaxis(C,C-b,theta);
	c.rotateaxis(C,C-b,theta);
	
}
// tilt
void PPC::tilt(float theta){
	b.rotateaxis(C,C+a,theta);
	c.rotateaxis(C,C+a,theta);
}
// roll
void PPC::roll(float theta){
	a.rotateaxis(C,C+a^b,theta);
	b.rotateaxis(C,C+a^b,theta);
	c.rotateaxis(C,C+a^b,theta);
}
// get focal length
float PPC::GetF(){
	V3 vd=a^b;
	vd.normalize();
	return vd*c;
}
// zoom
void PPC::zoom(float zoom){
	V3 vd=a^b;
	c=vd*GetF()*zoom-a*(float)w/2.0f-b*(float)h/2.0f;
}
// camera interpolation
void PPC::SetByInterpolation(PPC *ppc0,PPC *ppc1,int i,int n){
	V3 vd0 =ppc0->a^ppc0->b;
	vd0.normalize();
	V3 vd1 =ppc1->a^ppc1->b;
	vd1.normalize();
	C=ppc0->C+(ppc1->C-ppc0->C)*(float)i/(float)(n-1);
	V3 vd=vd0+(vd1-vd0)*(float)i/(float)(n-1);
	a=ppc0->a+(ppc1->a-ppc0->a)*(float)i/(float)(n-1);
	b=(vd^a);
	b.normalize();
	b=b*(V3::vectorMag(b));
	c=vd*GetF()-a*(float)w/2.0f-b*(float)h/2.0f;
	
}
void PPC::loadText(char *fname) {
	 int w,h;
	 string x,y,z;
	 ifstream myfile(fname,ios_base::in);
	 myfile>>w;
	 myfile>>h;
    
	 myfile>>x >> y>> z;
	 a=V3((float)atof(x.c_str()),(float)atof(y.c_str()),(float)atof(z.c_str()));

	 myfile>>x>>y>>z;
	 b=V3((float)atof(x.c_str()),(float)atof(y.c_str()),(float)atof(z.c_str()));
   
	 myfile>>x>>y>>z;
	 c=V3((float)atof(x.c_str()),(float)atof(y.c_str()),(float)atof(z.c_str()));
	 cout<<w<<endl;
	 cout<<h<<endl;
	 cout<<a<<endl;
	 cout<<b<<endl;
	 cout<<c<<endl;
	 cout<<C<<endl;
     cout << "" << endl;
     myfile.close();
     }
     
void PPC::saveText(char *fname){  
  
       ofstream myfile;
	   myfile.open(fname);
	   myfile<<w<<" "<<h<<endl;
	   myfile<<a<<endl;
	   myfile<<b<<endl;
	   myfile<<c<<endl;
	   myfile<<C<<endl;


      

       myfile.close();    
              
}