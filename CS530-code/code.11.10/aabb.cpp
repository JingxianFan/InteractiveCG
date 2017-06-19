#include "aabb.h"


AABB::AABB(V3 firstPoint) {

  corners[0] = firstPoint;
  corners[1] = firstPoint;

}

void AABB::AddPoint(V3 newPoint) {

  for (int i = 0; i < 3; i++) {
    if (newPoint[i] > corners[1][i])
      corners[1][i] = newPoint[i];
    if (newPoint[i] < corners[0][i])
      corners[0][i] = newPoint[i];
  }

}

bool AABB::ClipWithFrame(float left, float top, float right, float bottom) {


  // entire 2D AABB is off screen
  if (
    corners[0][0] >= right ||
    corners[1][0] <= left ||
    corners[0][1] >= bottom ||
    corners[1][1] <= top
    )
    return false;

  if (corners[0][0] < left)
    corners[0][0] = left;
  if (corners[1][0] > right)
    corners[1][0] = right;
  if (corners[0][1] < top)
    corners[0][1] = top;
  if (corners[1][1] > bottom)
    corners[1][1] = bottom;

  return true;

}

void AABB::SetPixelRectangle(int& left, int& right, int& top, int& bottom) {

  left = (int) (corners[0][0]+0.5f);
  right = (int) (corners[1][0]-0.5f);
  top = (int) (corners[0][1]+0.5f);
  bottom = (int) (corners[1][1]-0.5f);

}
