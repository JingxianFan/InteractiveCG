#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "ppc.h"
#include "TMesh.h"
#include "cubeMap.h"

#include <iostream>

class cubeMap {
public:
	 cubeMap(FrameBuffer *fb);//construct
	 FrameBuffer *face0, *face1, *face2, *face3, *face4, *face5;
	 FrameBuffer *currface;
	 PPC *ppc, *ppc1, *ppc2, *ppc3, *ppc0,*ppc4, *ppc5;
	
	unsigned int cubeGetColor(V3 direc);
};