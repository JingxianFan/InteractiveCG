#include "framebuffer.h"
#include <iostream>
#include "scene.h"
#include <math.h>
#include "v3.h"
#include "tiffio.h"
#include "tiff.h"


using namespace std;

// makes an OpenGL window that supports SW, HW rendering, that can be displayed on screen
//        and that receives UI events, i.e. keyboard, mouse, etc.
FrameBuffer::FrameBuffer(int u0, int v0, 
  int _w, int _h) : Fl_Gl_Window(u0, v0, _w, _h, 0) {

  w = _w;
  h = _h;
  pix = new unsigned int[w*h];

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
//draw rectangle
void FrameBuffer::DrawRec(float cuf,float cvf,float _w,float _h,unsigned int color){
	 // axis aligned bounding box (AABB) of rect
    int top = (int) (cvf + 0.5f);
    int bottom = (int) (cvf +_h - 0.5f);
    int left = (int) (cuf  + 0.5f);
    int right = (int) (cuf + _w - 0.5f);

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


    for (int v = top; v <= bottom; v++) {
      for (int u = left; u <= right; u++) {
        Set(u, v, color);
	  }
	}
}
// point in triangle
bool FrameBuffer::pointInTriangle(float u1,float v1,float u2,float v2,float u3,float v3,float u,float v){

	V3 A(u1,v1,0);
	V3 B(u2,v2,0);
	V3 C(u3,v3,0);
	V3 P(u,v,0);
	V3 V0=C-A;
	V3 V1=B-A;
	V3 V2=P-A;

	float dot00= V0*V0;
	float dot01= V0*V1;
	float dot02= V0*V2;
	float dot11= V1*V1;
	float dot12= V1*V2;
	
	float inverDeno =1/(dot00*dot11-dot01*dot01);

	float x=(dot11*dot02-dot01*dot12)*inverDeno;
	if(x<0||x>1){
		return true;
	}
	float y=(dot00*dot12-dot01*dot02)*inverDeno;
	if(y<0||y>1){
		return true;
	}
	return x+y>1;
}

// draw triangle
void FrameBuffer::DrawTriangle(float u1,float v1,float u2,float v2,float u3,float v3,unsigned int color){
	float vtop =min(v1,min(v2,v3));
	float vbottom =max(v1,max(v2,v3));
	float uleft =min(u1,min(u2,u3));
	float uright =max(u1,max(u2,u3));
	 // axis aligned bounding box (AABB) of triangle
	int top = (int) (vtop + 0.5f);
	int bottom = (int) ( vbottom- 0.5f);
	int left = (int) (uleft  + 0.5f);
	int right = (int) (uright - 0.5f);
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
	
    for (int v = top; v <= bottom; v++) {
      for (int u = left; u <= right; u++) {
        if (pointInTriangle(u1,v1,u2,v2,u3,v3,u,v))
          continue;
        Set(u, v, color);
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

// load tiff
void FrameBuffer::loadtiff(char* file){
	TIFF* tif= TIFFOpen(file,"r");
	if(tif){
		uint32 w,h;
		size_t npixels;
		uint32* raster;

		TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&w);
		TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&h);
		npixels=w*h;
		raster=(uint32*)_TIFFmalloc(npixels*sizeof(uint32));
		if(raster!=NULL){
			if(TIFFReadRGBAImage(tif,w,h,raster,0)){

				delete pix;
				pix=raster;
			
			
			}
		}
	}	


}
// save tiff
int FrameBuffer::savetiff(char* file){
  int i,j;
  long stripsperimage = 0;
  long rasterbase;
  unsigned char *tmpStorage;
  unsigned char *tmpPtr;
  TIFF* tif = TIFFOpen(file, "w");
  if (tif) {
	int ncol = w;
	int nrow = h;
	unsigned int* raster = pix;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, ncol);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, nrow);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, 1); /* none */
    TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
    TIFFSetField(tif, TIFFTAG_XRESOLUTION, 1200.0);
    TIFFSetField(tif, TIFFTAG_YRESOLUTION, 1200.0);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

    /* calculate the number of strips we want in the TIFF file */
    stripsperimage = nrow * ncol / 8000;
    if(stripsperimage < 1)
      stripsperimage = 1;
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, stripsperimage);

    tmpStorage = (unsigned char *)malloc(sizeof(unsigned char) * ncol * 3);
    if(!tmpStorage) {
      fprintf(stderr, "Memory allocation error\n");
      TIFFClose(tif);
      return(-1);
    }
    if (raster != NULL) {
      for(i=0;i<nrow;i++) {
	rasterbase = (nrow - i - 1) * ncol;
	for(j=0;j<ncol*3;) {
	  tmpPtr = (unsigned char *)&(raster[rasterbase]);
	  tmpStorage[j] = tmpPtr[3];
	  j++;
	  tmpStorage[j] = tmpPtr[2];
	  j++;
	  tmpStorage[j] = tmpPtr[1];
	  j++;
	  rasterbase++;
	}
	if (TIFFWriteScanline(tif, tmpStorage, i, 0) != 1) {
	  fprintf(stderr, "Unable to write scanline %d\n",i);
	  return(-1);
	}
      }

      TIFFClose(tif);
      return(0);
    }
    else {
      TIFFClose(tif);
      fprintf(stderr, "image is NULL\n");
      return(-1);
    }
  }
  else {
    fprintf(stderr, "Unable to open %s for writing\n",file);
    return(-1);
  }
}
