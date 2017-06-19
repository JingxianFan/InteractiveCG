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
