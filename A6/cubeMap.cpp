
#include "cubeMap.h"
#include <iostream>
#include <math.h>

using namespace std;


cubeMap::cubeMap(FrameBuffer *fb){

			 float height,width;
			 height=int(float(fb->h)/3);
			 width=int(float(fb->w)/4);

			ppc= new PPC(90.0f,width,height);
	
			ppc2= new PPC(90.0f,ppc->w,ppc->h);
		
		ppc0= new PPC(90.0f,ppc->w,ppc->h);
		ppc0->Pan(90.0f);
		ppc4= new PPC(90.0f,ppc->w,ppc->h);
		ppc4->Pan(-90.0f);
		ppc1= new PPC(90.0f,ppc->w,ppc->h);
		ppc1->Tilt(90.0f);
		ppc3= new PPC(90.0f,ppc->w,ppc->h);
		ppc3->Tilt(-90.0f);
		ppc5= new PPC(90.0f,ppc->w,ppc->h);
		ppc5->Pan(180.0f);

			 face0= new FrameBuffer(20,50,width,height);
			 for(int u=0;u<face0->w;u++){
				 for(int v=0;v<face0->h;v++){
					int uv=(face0->h-v-1)*face0->w+u;
					int fb_uv=(fb->h-(v+height)-1)*fb->w+u;
					face0->pix[uv]=fb->pix[fb_uv];
				 }
			 }

			 face1= new FrameBuffer(20,50,width,height);
			 for(int u=0;u<face1->w;u++){
				 for(int v=0;v<face1->h;v++){
					int uv=(face1->h-v-1)*face1->w+u;
					int fb_uv=(fb->h-v-1)*fb->w+(u+width);
					face1->pix[uv]=fb->pix[fb_uv];
				 }
			 }
			 face2= new FrameBuffer(20,50,width,height);
			 for(int u=0;u<face2->w;u++){
				 for(int v=0;v<face2->h;v++){
					int uv=(face2->h-v-1)*face2->w+u;
					int fb_uv=(fb->h-(v+height)-1)*fb->w+(u+width);
					face2->pix[uv]=fb->pix[fb_uv];
				 }
			 }
			 face3= new FrameBuffer(20,50,width,height);
			 for(int u=0;u<face3->w;u++){
				 for(int v=0;v<face3->h;v++){
					int uv=(face3->h-v-1)*face3->w+u;
					int fb_uv=(fb->h-(v+2*height)-1)*fb->w+(u+width);
					face3->pix[uv]=fb->pix[fb_uv];
				 }
			 }
			 face4= new FrameBuffer(20,50,width,height);
			 for(int u=0;u<face4->w;u++){
				 for(int v=0;v<face4->h;v++){
					int uv=(face4->h-v-1)*face4->w+u;
					int fb_uv=(fb->h-(v+height)-1)*fb->w+(u+2*width);
					face4->pix[uv]=fb->pix[fb_uv];
				 }
			 }
			 face5= new FrameBuffer(20,50,width,height);
			 for(int u=0;u<face5->w;u++){
				 for(int v=0;v<face5->h;v++){
					int uv=(face5->h-v-1)*face5->w+u;
					int fb_uv=(fb->h-(v+height)-1)*fb->w+(u+3*width);
					face5->pix[uv]=fb->pix[fb_uv];
				 }
			 }
			

};
unsigned int cubeMap::cubeGetColor(V3 direc){
	V3 projD;
	int uv;
	float x,y;
	
	
	if(ppc->Project(direc,projD)){
		
		if(0<=projD[1] && projD[1]<=ppc->h && 0<=projD[0] && projD[0]<=ppc->w){
			
		    x=(projD[0])/float(ppc2->w)*face2->w;
			y=(projD[1])/float(ppc2->h)*face2->h;
			uv =(face2->h-int(y)-1)*face2->w+int(x);
			if(uv>=0 && uv<=face2->h*face2->w)
			return face2->pix[uv];
			
		}
		else if(projD[0]<0 && projD[1]>projD[0]&& projD[1]<ppc->w-projD[0]){
			ppc0->Project(direc,projD);

		    x=(projD[0])/float(ppc0->w)*face0->w;
			y=(projD[1])/float(ppc0->h)*face0->h;
			uv =(face0->h-int(y)-1)*face0->w+int(x);	
			if(uv>=0 && uv<=face0->h*face0->w)
				return face0->pix[uv];
		}
		else if(projD[0]>ppc->w && projD[0]>projD[1] && projD[1]>ppc->w-projD[0]){
			ppc4->Project(direc,projD);
		    x=(projD[0])/float(ppc4->w)*face4->w;
			y=(projD[1])/float(ppc4->h)*face4->h;
			uv =(face4->h-int(y)-1)*face4->w+int(x);	
			if(uv>=0 && uv<=face4->h*face4->w)
			return face4->pix[uv];
			}
		else if(projD[1]<0 && projD[1]<projD[0] && projD[1]<ppc->w-projD[0]){
			ppc1->Project(direc,projD);
			x=(projD[0])/float(ppc1->w)*face1->w;
			y=(projD[1])/float(ppc1->h)*face1->h;
			uv =(face1->h-int(y)-1)*face1->w+int(x);
			if(uv>=0 && uv<=face1->h*face1->w)
			return face1->pix[uv];
		}
		else if(projD[1]>ppc->h && projD[1]>ppc->w-projD[0] && projD[1]>projD[0]){
			ppc3->Project(direc,projD);
			x=projD[0]/float(ppc3->w)*face3->w;
			y=(projD[1])/float(ppc3->h)*face3->h;
			uv =(face3->h-int(y)-1)*face3->w+int(x);
			if(uv>=0 && uv<=face3->h*face3->w)
			return face3->pix[uv];
		}
	}
	else if(ppc5->Project(direc,projD)){
			if(0<=projD[1] && projD[1]<=ppc5->h && 0<=projD[0] && projD[0]<=ppc5->w){
				x=projD[0]/float(ppc5->w)*face5->w;
				y=(projD[1])/float(ppc5->h)*face5->h;
				uv =(face5->h-int(y)-1)*face5->w+int(x);
				if(uv>=0 && uv<=face5->h*face5->w)
				return face5->pix[uv];
			}
			else if(projD[0]<0 && projD[1]>projD[0]&& projD[1]<ppc->w-projD[0]){
				ppc4->Project(direc,projD);
				x=(projD[0])/float(ppc4->w)*face4->w;
				y=(projD[1])/float(ppc4->h)*face4->h;
				uv =(face4->h-int(y)-1)*face4->w+int(x);	
				if(uv>=0 && uv<=face4->h*face4->w)
				return face4->pix[uv];
			}
			else if(projD[0]>ppc->w && projD[0]>projD[1] && projD[1]>ppc->w-projD[0]){
				ppc0->Project(direc,projD);
				x=(projD[0])/float(ppc0->w)*face0->w;
				y=(projD[1])/float(ppc0->h)*face0->h;
				uv =(face0->h-int(y)-1)*face0->w+int(x);	
				if(uv>=0 && uv<=face0->h*face0->w)
				return face0->pix[uv];
			}
			else if(projD[1]<0 && projD[1]<projD[0] && projD[1]<ppc->w-projD[0]){
				ppc1->Project(direc,projD);
				x=(projD[0])/float(ppc1->w)*face1->w;
				y=(projD[1])/float(ppc1->h)*face1->h;
				uv =(face1->h-int(y)-1)*face1->w+int(x);
				if(uv>=0 && uv<=face1->h*face1->w)
				return face1->pix[uv];
			}
			else if(projD[1]>ppc->h && projD[1]>ppc->w-projD[0] && projD[1]>projD[0]){
				ppc3->Project(direc,projD);
				x=projD[0]/float(ppc3->w)*face3->w;
				y=(projD[1])/float(ppc3->h)*face3->h;
				uv =(face3->h-int(y)-1)*face3->w+int(x);
				if(uv>=0 && uv<=face3->h*face3->w)
				return face3->pix[uv]; 
			}
		}
};