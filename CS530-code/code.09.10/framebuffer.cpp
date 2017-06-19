#include "framebuffer.h"
#include <iostream>
#include "scene.h"
#include <math.h>

#include "tiffio.h"

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

// save as tiff image
void FrameBuffer::SaveAsTiff(char *fname) {

  TIFF* out = TIFFOpen(fname, "w");

  if(out == NULL) {
    cerr << fname << " could not be opened" << endl;
    return;
  }

  cerr << "INFO: yet to be implemented" << endl;
  
  TIFFClose(out);

}
