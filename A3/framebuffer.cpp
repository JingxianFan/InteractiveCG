#include "framebuffer.h"
#include <iostream>
#include "scene.h"
#include <math.h>
#include "m33.h"
#include "ppc.h"

#include "tiffio.h"

using namespace std;


// makes an OpenGL window that supports SW, HW rendering, that can be displayed on screen
//        and that receives UI events, i.e. keyboard, mouse, etc.
FrameBuffer::FrameBuffer(int u0, int v0, 
  int _w, int _h) : Fl_Gl_Window(u0, v0, _w, _h, 0) {

  w = _w;
  h = _h;
  pix = new unsigned int[w*h];
  zb = new float[w*h];

}

// rendering callback; see header file comment
void FrameBuffer::draw() {

	// SW window, just transfer computed pixels from pix to HW for display
	glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, pix);

}


// function called automatically on event within window (callback)
int FrameBuffer::handle(int event)  {  

  switch(event) 
  {   
  case FL_KEYBOARD: {
    KeyboardHandle();
    return 0;
  }
  default:
    break;
  }
  return 0;

}


void FrameBuffer::KeyboardHandle() {

  int key = Fl::event_key();
  switch (key) {
    case FL_Left: {
      cerr << "pressed left arrow" << endl;
      break;
    }
    case 'a': {
      cerr << "pressed a" << endl;
      break;
    }
    default:
    cerr << "INFO: do not understand keypress" << endl;
  }
}

// clear to background color
void FrameBuffer::Set(unsigned int color) {

  for (int i = 0; i < w*h; i++) {
    pix[i] = color;
  }

}
unsigned int FrameBuffer::GetTexel(float currs,float currt,FrameBuffer *fb){
	int u,v;
	u=currs* fb->w;
	v=currt* fb->h;
	unsigned int texel=fb->pix[int(( fb->h -1-v)*fb->w+u)];
	return texel;
}
void FrameBuffer::SetTexture(int u,int v,unsigned int texel){
	int i=(h-1-v)*w+u;
    pix[i]=texel;

}
// clear to far
void FrameBuffer::ClearZB(float farz) {

  for (int i = 0; i < w*h; i++) {
    zb[i] = farz;
  }

}

// set pixel with coordinates u v to color provided as parameter
void FrameBuffer::SetSafe(int u, int v, unsigned int color) {

  if (u < 0 || u > w-1 || v < 0 || v > h-1)
    return;

  Set(u, v, color);

}

void FrameBuffer::Set(int u, int v, unsigned int color) {

  pix[(h-1-v)*w+u] = color;

}

// set to checkboard
void FrameBuffer::SetCheckerboard(int checkerSize, unsigned int color0, 
  unsigned int color1) {

  for (int v = 0; v < h; v++) {
    for (int u = 0; u < w; u++) {
      int cu = u / checkerSize;
      int cv = v / checkerSize;
      if (((cu + cv) % 2) == 0) {
        Set(u, v, color0);
      }
      else {
        Set(u, v, color1);
      }
    }
  }

}

// draw circle
void FrameBuffer::DrawCircle(float cuf, float cvf, float radius, 
  unsigned int color) {

    // axis aligned bounding box (AABB) of circle (it's a square parallel to axes)
    int top = (int) (cvf - radius + 0.5f);
    int bottom = (int) (cvf + radius - 0.5f);
    int left = (int) (cuf - radius + 0.5f);
    int right = (int) (cuf + radius - 0.5f);

    // clip AABB with frame
    if (top > h-1 || bottom < 0 || right < 0 || left > w-1)
      return;
    if (left < 0)
      left = 0;
    if (right > w-1)
      right = w-1;
    if (top < 0)
      top = 0;
    if (bottom > h-1)
      bottom = h-1;

    float radius2 = radius*radius;
    for (int v = top; v <= bottom; v++) {
      for (int u = left; u <= right; u++) {
        float uf = .5f + (float)u;
        float vf = .5f + (float)v;
        float d2 = (cvf-vf)*(cvf-vf)+(cuf-uf)*(cuf-uf);
        if (d2 > radius2)
          continue;
        Set(u, v, color);
      }
    }

}
void FrameBuffer::DrawTextureTri(V3 *vs,PPC *ppc,V3 s,V3 t,FrameBuffer *fb, V3 ns,V3 lightPosition, float ambientk,float kp){
 
  V3 pvs[3];
  for(int i=0;i<3;i++){
	ppc->Project(vs[i],pvs[i]);
  }
  // project vertices
 
  AABB aabb(pvs[0]);
  aabb.AddPoint(pvs[1]);
  aabb.AddPoint(pvs[2]);

  if (!aabb.ClipWithFrame(0.0f, 0.0f, (float) w, (float) h))
    return;

  int left, right, top, bottom;
  aabb.SetPixelRectangle(left, right, top, bottom);
  // set edge equations
  V3 eeqs[3]; // eeqs[0] = (A, B, C), where Au + Bv + C
  for (int ei = 0; ei < 3; ei++) {
    int e1 = (ei+1)%3;
    eeqs[ei][0] = pvs[e1][1]-pvs[ei][1];
    eeqs[ei][1] = pvs[ei][0]-pvs[e1][0];
    eeqs[ei][2] = -pvs[ei][1]*eeqs[ei][1]-pvs[ei][0]*eeqs[ei][0];
    int e2 = (e1+1)%3;
    V3 pv3(pvs[e2][0], pvs[e2][1], 1.0f); // (u2, v2, 1)
    if (eeqs[ei]*pv3 < 0.0f)
      eeqs[ei] = eeqs[ei]*-1.0f;
  }
   // set Model Interpolation
  M33 q;
  q.SetColumn(vs[0]-ppc->C,0);
  q.SetColumn(vs[1]-ppc->C,1);
  q.SetColumn(vs[2]-ppc->C,2);
  q = q.Inverted();

  M33 abc;
  abc.SetColumn(ppc->a,0);
  abc.SetColumn(ppc->b,1);
  abc.SetColumn(ppc->c,2);
  
  M33 Q;
  Q[0][0]=q[0]*abc.GetColumn(0);
  Q[0][1]=q[0]*abc.GetColumn(1);
  Q[0][2]=q[0]*abc.GetColumn(2);
  Q[1][0]=q[1]*abc.GetColumn(0);
  Q[1][1]=q[1]*abc.GetColumn(1);
  Q[1][2]=q[1]*abc.GetColumn(2);
  Q[2][0]=q[2]*abc.GetColumn(0);
  Q[2][1]=q[2]*abc.GetColumn(1);
  Q[2][2]=q[2]*abc.GetColumn(2);
  V3 unit(1,1,1);
  float _sA=Q.GetColumn(0)*s;
  float _sB=Q.GetColumn(1)*s;
  float _sC=Q.GetColumn(2)*s;
  float _sD=Q.GetColumn(0)*unit;
  float _sE=Q.GetColumn(1)*unit;
  float _sF=Q.GetColumn(2)*unit;

  float _tA=Q.GetColumn(0)*t;
  float _tB=Q.GetColumn(1)*t;
  float _tC=Q.GetColumn(2)*t;
  float _tD=Q.GetColumn(0)*unit;
  float _tE=Q.GetColumn(1)*unit;
  float _tF=Q.GetColumn(2)*unit;



  for (int v = top; v <= bottom; v++) {
    for (int u = left; u <= right; u++) {
	  V3 cvs=(ppc->a*u+ppc->b*v+ppc->c);
	  V3 lightVector=(lightPosition.UnitVector() - cvs.UnitVector()).UnitVector();
      V3 pixv(.5f+(float)u, .5f+(float)v, 1.0f); // pixel center
      
      if (
        pixv*eeqs[0] < 0.0f ||
        pixv*eeqs[1] < 0.0f ||
        pixv*eeqs[2] < 0.0f)
        continue; // pixel center is on wrong side of any of 3 edges, discard

      float currs =(_sA*pixv[0]+_sB*pixv[1]+_sC*pixv[2])/(_sD*pixv[0]+_sE*pixv[1]+_sF*pixv[2]);
	  float currt =(_tA*pixv[0]+_tB*pixv[1]+_tC*pixv[2])/(_tD*pixv[0]+_tE*pixv[1]+_tF*pixv[2]);
      unsigned int texel=GetTexel(currs,currt,fb);
	  float kd = lightVector*(ns);
		if (kd < 0.0f)
			kd = 0.0f;
	  int r,g,b;
	  r = int(((unsigned char*)(&texel))[0]);
	  g = int(((unsigned char*)(&texel))[1]);
	  b = int(((unsigned char*)(&texel))[2]);
	  V3 color(r/255.0,g/255.0,b/255.0);
	  color = color*(ambientk + kd*(1.0f-ambientk)+kp);
	 
	  texel=color.GetColor();
      SetTexture(u,v,texel);
	 
    }
  }
 
}
// load a tiff image to pixel buffer
void FrameBuffer::LoadTiff(char* fname) {
  TIFF* in = TIFFOpen(fname, "r");
  if (in == NULL) {
	cerr << fname << " could not be opened" << endl;
	return;
  }

  int width, height;
  TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
  TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
  if (w != width || h != height) {
	w = width;
	h = height;
	delete[] pix;
	delete[] zb;
	pix = new unsigned int[w*h];
	zb = new float[w*h];
	size(w, h);
	glFlush();
	glFlush();
  }

  if (TIFFReadRGBAImage(in, w, h, pix, 0) == 0) {
	cerr << "failed to load " << fname << endl;
  }

  TIFFClose(in);
}

// save as tiff image
void FrameBuffer::SaveAsTiff(char *fname) {

  TIFF* out = TIFFOpen(fname, "w");

  if(out == NULL) {
    cerr << fname << " could not be opened" << endl;
    return;
  }

  TIFFSetField(out, TIFFTAG_IMAGEWIDTH, w);
  TIFFSetField(out, TIFFTAG_IMAGELENGTH, h);
  TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
  TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
  TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
  TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

  for (uint32 row = 0; row < h; row++) {
	TIFFWriteScanline(out, &pix[(h - row - 1) * w], row);
  }
  
  TIFFClose(out);
}

// draw vertex dots
void FrameBuffer::DrawVertexDots(TMesh *tm, int dotSize, PPC *ppc) {

  for (int vi = 0; vi < tm->vertsN; vi++) {
    V3 projV, projP;
    if (!ppc->Project(tm->verts[vi], projP))
      continue;
    DrawCircle(projP[0], projP[1], dotSize, tm->cols[vi].GetColor());
  }

}

void FrameBuffer::DrawBilinear(V3 *vs,PPC *ppc,FrameBuffer *fb){

  V3 pvs[4];
  for(int i=0;i<4;i++){
	ppc->Project(vs[i],pvs[i]);
  }
  AABB aabb(pvs[0]);
  aabb.AddPoint(pvs[1]);
  aabb.AddPoint(pvs[2]);
  aabb.AddPoint(pvs[3]);

  if (!aabb.ClipWithFrame(0.0f, 0.0f, (float) w, (float) h))
    return;

  int left, right, top, bottom;
  aabb.SetPixelRectangle(left, right, top, bottom);


  for (int v = top; v <= bottom; v++) {
   for (int u = left; u <= right; u++) {
	    V3 pixv(.5f+(float)u, .5f+(float)v, 1.0f);
		 float currs=(float)u/(float)(w);
		 float currt=(float)(h-v)/(float)(h);
		 unsigned int texel=GetTexel(currs,currt,fb);
		 SetTexture(u,v,texel);

   }
  }

}
// draw TMesh in wire frame
void FrameBuffer::DrawWireFrame(TMesh *tm, PPC *ppc) {

  for (int tri = 0; tri < tm->trisN; tri++) {
    V3 currvs[3];
    currvs[0] = tm->verts[tm->tris[3*tri+0]];
    currvs[1] = tm->verts[tm->tris[3*tri+1]];
    currvs[2] = tm->verts[tm->tris[3*tri+2]];
    V3 currcols[3];
    currcols[0] = tm->cols[tm->tris[3*tri+0]];
    currcols[1] = tm->cols[tm->tris[3*tri+1]];
    currcols[2] = tm->cols[tm->tris[3*tri+2]];
    for (int ei = 0; ei < 3; ei++) {
      Draw3DSegment(currvs[ei], currcols[ei], 
        currvs[(ei+1)%3], currcols[(ei+1)%3], ppc);
    }
  }

}

// draw TMesh in filled mode
void FrameBuffer::DrawFilledMode(TMesh *tm, PPC *ppc,
  V3 lightPosition, float ambientk) {

  // project vertices
  tm->ProjectVertices(ppc);

  for (int tri = 0; tri < tm->trisN; tri++) {
    V3 currvs[3];
    currvs[0] = tm->verts[tm->tris[3*tri+0]];
    currvs[1] = tm->verts[tm->tris[3*tri+1]];
    currvs[2] = tm->verts[tm->tris[3*tri+2]];
    V3 currns[3];
    V3 *ns = 0;
    if (tm->normals) {
      currns[0] = tm->normals[tm->tris[3*tri+0]];
      currns[1] = tm->normals[tm->tris[3*tri+1]];
      currns[2] = tm->normals[tm->tris[3*tri+2]];
      ns = currns;
    }
    V3 projcurrvs[3];
    projcurrvs[0] = tm->projVerts[tm->tris[3*tri+0]];
    projcurrvs[1] = tm->projVerts[tm->tris[3*tri+1]];
    projcurrvs[2] = tm->projVerts[tm->tris[3*tri+2]];
    V3 currcols[3];
    currcols[0] = tm->cols[tm->tris[3*tri+0]];
    currcols[1] = tm->cols[tm->tris[3*tri+1]];
    currcols[2] = tm->cols[tm->tris[3*tri+2]];
    DrawProjectedTriangle(currvs, projcurrvs, currcols, currns, lightPosition,
      ambientk);
  }

}

V3 FrameBuffer::Light(V3 matColor, float ka, V3 lv, V3 n) {

  float kd = lv*n;
  if (kd < 0.0f)
    kd = 0.0f;
  // kd goes from 0 to 1
  
  // let's say ka = 0.4, then ret should go from matColor to 0.4*matColor
  V3 ret = matColor* (ka + kd*(1.0f-ka));

  return ret;

}

void FrameBuffer::DrawProjectedTriangle(V3 *vs, V3 *pvs, V3 *cols, V3 *ns,
  V3 lightPosition, float ambientk) {

  AABB aabb(pvs[0]);
  aabb.AddPoint(pvs[1]);
  aabb.AddPoint(pvs[2]);

  if (!aabb.ClipWithFrame(0.0f, 0.0f, (float) w, (float) h))
    return;

  int left, right, top, bottom;
  aabb.SetPixelRectangle(left, right, top, bottom);

  // set edge equations
  V3 eeqs[3]; // eeqs[0] = (A, B, C), where Au + Bv + C
  for (int ei = 0; ei < 3; ei++) {
    int e1 = (ei+1)%3;
    eeqs[ei][0] = pvs[e1][1]-pvs[ei][1];
    eeqs[ei][1] = pvs[ei][0]-pvs[e1][0];
    eeqs[ei][2] = -pvs[ei][1]*eeqs[ei][1]-pvs[ei][0]*eeqs[ei][0];
    int e2 = (e1+1)%3;
    V3 pv3(pvs[e2][0], pvs[e2][1], 1.0f); // (u2, v2, 1)
    if (eeqs[ei]*pv3 < 0.0f)
      eeqs[ei] = eeqs[ei]*-1.0f;
  }

  // set Screen Space Interpolation
  M33 ssim;
  ssim[0] = pvs[0];
  ssim[1] = pvs[1];
  ssim[2] = pvs[2];
  ssim.SetColumn(V3(1.0f, 1.0f, 1.0f), 2);
  ssim = ssim.Inverted();
  // linear expression for screen space interpolation of 1/w
  V3 zABC = ssim*V3(pvs[0][2], pvs[1][2], pvs[2][2]);

  // lighting
  V3 matColor = V3(1.0f, 0.0f, 0.0f);
  for (int vi = 0; vi < 3; vi++) {
    V3 lightVector = (lightPosition - vs[vi]).UnitVector();
    cols[vi] = Light(matColor, ambientk, lightVector, ns[vi]);
  }

  // linear expressions for screen space interpolation of colors
  M33 colsABC;
  colsABC[0] = ssim*V3(cols[0][0], cols[1][0], cols[2][0]);
  colsABC[1] = ssim*V3(cols[0][1], cols[1][1], cols[2][1]);
  colsABC[2] = ssim*V3(cols[0][2], cols[1][2], cols[2][2]);


  V3 color = (cols[0] + cols[1] + cols[2]) / 3.0f;

  for (int v = top; v <= bottom; v++) {
    for (int u = left; u <= right; u++) {

      V3 pixv(.5f+(float)u, .5f+(float)v, 1.0f); // pixel center
      
      if (
        pixv*eeqs[0] < 0.0f ||
        pixv*eeqs[1] < 0.0f ||
        pixv*eeqs[2] < 0.0f)
        continue; // pixel center is on wrong side of any of 3 edges, discard

      float currz = zABC * pixv; // z at current pixel interpolated lin. in s s
      V3 currCol = colsABC*pixv; // color at current pixel interp. l s s
      SetIfCloser(V3(pixv[0], pixv[1], currz), currCol);

    }
  }

}

void FrameBuffer::Draw3DSegment(V3 v0, V3 c0, V3 v1, V3 c1, PPC *ppc) {

  V3 projv0, projv1;
  if (!ppc->Project(v0, projv0))
    return;
  if (!ppc->Project(v1, projv1))
    return;
  Draw2DSegment(projv0, c0, projv1, c1);

}

void FrameBuffer::Draw2DSegment(V3 v0, V3 c0, V3 v1, V3 c1) {

  int stepsN;
  float duf = fabsf(v0[0] - v1[0]);
  float dvf = fabsf(v0[1] - v1[1]);
  if (duf > dvf) {
    stepsN = 2 + (int) duf;
  }
  else {
    stepsN = 2 + (int) dvf;
  }

  for (int i = 0; i < stepsN; i++) {
    float frac = (float) i / (float) (stepsN - 1);
    V3 p = v0 + (v1-v0)*frac;
    V3 c = c0 + (c1-c0)*frac;
    SetIfCloser(p, c);
//    SetSafe((int)p[0], (int)p[1], c.GetColor());
  }

}

// sets pixel at p[0], p[1] to color c only if p[2] is closer than zb value at 
//        that pixel
void FrameBuffer::SetIfCloser(V3 p, V3 c) {

  if (p[0] < 0.0f || p[0] >= w || p[1] < 0.0f || p[1] >= h)
    return;

  int u = (int) p[0];
  int v = (int) p[1];

  if (zb[(h-1-v)*w+u] >= p[2])
    return; // nothing to draw, already saw a surface closer at that pixel

  zb[(h-1-v)*w+u] = p[2]; // set z at pixel to new closest surface value
  Set(u, v, c.GetColor());

}


void FrameBuffer::Draw3DSegmentNaive(V3 v0, V3 c0, V3 v1, V3 c1, PPC *ppc) {

  int stepsN = 300;
  for (int i = 0; i < stepsN; i++) {
    float frac = (float)i / (float) (stepsN-1);
    V3 P = v0 + (v1-v0)*frac;
    V3 col = c0 + (c1-c0)*frac;
    V3 projP;
    if (!ppc->Project(P, projP))
      continue;
    DrawCircle(projP[0], projP[1], 1, col.GetColor());
  }

}

// Draw AABB
void FrameBuffer::DrawAABB(AABB aabb, unsigned int colorNear, 
  unsigned int colorFar, PPC *ppc) {

  V3 cnear;
  cnear.SetFromColor(colorNear);
  V3 cfar;
  cfar.SetFromColor(colorFar);

  V3 p0, p1;

  p0 = V3(aabb.corners[0][0], aabb.corners[1][1], aabb.corners[1][2]); 
  p1 = V3(aabb.corners[0][0], aabb.corners[0][1], aabb.corners[1][2]); 
  Draw3DSegment(p0, cnear, p1, cnear, ppc);

  p0 = V3(aabb.corners[0][0], aabb.corners[0][1], aabb.corners[1][2]); 
  p1 = V3(aabb.corners[1][0], aabb.corners[0][1], aabb.corners[1][2]); 
  Draw3DSegment(p0, cnear, p1, cnear, ppc);

  p0 = V3(aabb.corners[1][0], aabb.corners[0][1], aabb.corners[1][2]); 
  p1 = V3(aabb.corners[1][0], aabb.corners[1][1], aabb.corners[1][2]); 
  Draw3DSegment(p0, cnear, p1, cnear, ppc);

  p0 = V3(aabb.corners[1][0], aabb.corners[1][1], aabb.corners[1][2]); 
  p1 = V3(aabb.corners[0][0], aabb.corners[1][1], aabb.corners[1][2]); 
  Draw3DSegment(p0, cnear, p1, cnear, ppc);


  p0 = V3(aabb.corners[0][0], aabb.corners[1][1], aabb.corners[0][2]); 
  p1 = V3(aabb.corners[0][0], aabb.corners[0][1], aabb.corners[0][2]); 
  Draw3DSegment(p0, cfar, p1, cfar, ppc);

  p0 = V3(aabb.corners[0][0], aabb.corners[0][1], aabb.corners[0][2]); 
  p1 = V3(aabb.corners[1][0], aabb.corners[0][1], aabb.corners[0][2]); 
  Draw3DSegment(p0, cfar, p1, cfar, ppc);

  p0 = V3(aabb.corners[1][0], aabb.corners[0][1], aabb.corners[0][2]); 
  p1 = V3(aabb.corners[1][0], aabb.corners[1][1], aabb.corners[0][2]); 
  Draw3DSegment(p0, cfar, p1, cfar, ppc);

  p0 = V3(aabb.corners[1][0], aabb.corners[1][1], aabb.corners[0][2]); 
  p1 = V3(aabb.corners[0][0], aabb.corners[1][1], aabb.corners[0][2]); 
  Draw3DSegment(p0, cfar, p1, cfar, ppc);


  p0 = V3(aabb.corners[0][0], aabb.corners[1][1], aabb.corners[1][2]); 
  p1 = V3(aabb.corners[0][0], aabb.corners[1][1], aabb.corners[0][2]); 
  Draw3DSegment(p0, cnear, p1, cfar, ppc);

  p0 = V3(aabb.corners[0][0], aabb.corners[0][1], aabb.corners[1][2]); 
  p1 = V3(aabb.corners[0][0], aabb.corners[0][1], aabb.corners[0][2]); 
  Draw3DSegment(p0, cnear, p1, cfar, ppc);

  p0 = V3(aabb.corners[1][0], aabb.corners[0][1], aabb.corners[1][2]); 
  p1 = V3(aabb.corners[1][0], aabb.corners[0][1], aabb.corners[0][2]); 
  Draw3DSegment(p0, cnear, p1, cfar, ppc);

  p0 = V3(aabb.corners[1][0], aabb.corners[1][1], aabb.corners[1][2]); 
  p1 = V3(aabb.corners[1][0], aabb.corners[1][1], aabb.corners[0][2]); 
  Draw3DSegment(p0, cnear, p1, cfar, ppc);

}

#if 0
  4        7

0-------3
| 5     |  6
|       | 
1-------2
#endif
