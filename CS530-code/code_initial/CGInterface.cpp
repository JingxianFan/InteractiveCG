#define GEOM_SHADER

#include "CGInterface.h"
#include "v3.h"
#include "scene.h"

#include <iostream>

using namespace std;

CGInterface::CGInterface() {};

void CGInterface::PerSessionInit() {

  glEnable(GL_DEPTH_TEST);

  CGprofile latestVertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
  CGprofile latestGeometryProfile = cgGLGetLatestProfile(CG_GL_GEOMETRY);
  CGprofile latestPixelProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);

  if (latestGeometryProfile == CG_PROFILE_UNKNOWN) {
    cerr << "ERROR: geometry profile is not available" << endl;
#ifdef GEOM_SHADER
    exit(0);
#endif
  }

  cgGLSetOptimalOptions(latestGeometryProfile);
  CGerror Error = cgGetError();
  if (Error) {
	  cerr << "CG ERROR: " << cgGetErrorString(Error) << endl;
  }

  cout << "Info: Latest GP Profile Supported: " << cgGetProfileString(latestGeometryProfile) << endl;

  geometryCGprofile = latestGeometryProfile;

  cout << "Info: Latest VP Profile Supported: " << cgGetProfileString(latestVertexProfile) << endl;
  cout << "Info: Latest FP Profile Supported: " << cgGetProfileString(latestPixelProfile) << endl;

  vertexCGprofile = latestVertexProfile;
  pixelCGprofile = latestPixelProfile;
  cgContext = cgCreateContext();  


}

bool ShaderOneInterface::PerSessionInit(CGInterface *cgi) {

#ifdef GEOM_SHADER
  geometryProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, 
    "CG/shaderOne.cg", cgi->geometryCGprofile, "GeometryMain", NULL);
  if (geometryProgram == NULL)  {
    CGerror Error = cgGetError();
    cerr << "Shader One Geometry Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
    cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
    return false;
  }
#endif

  vertexProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, 
    "CG/shaderOne.cg", cgi->vertexCGprofile, "VertexMain", NULL);
  if (vertexProgram == NULL) {
    CGerror Error = cgGetError();
    cerr << "Shader One Geometry Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
    cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
    return false;
  }

  fragmentProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, 
    "CG/shaderOne.cg", cgi->pixelCGprofile, "FragmentMain", NULL);
  if (fragmentProgram == NULL)  {
    CGerror Error = cgGetError();
    cerr << "Shader One Fragment Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
    cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
    return false;
  }

	// load programs
#ifdef GEOM_SHADER
	cgGLLoadProgram(geometryProgram);
#endif
	cgGLLoadProgram(vertexProgram);
	cgGLLoadProgram(fragmentProgram);

	// build some parameters by name such that we can set them later...
  vertexModelViewProj = cgGetNamedParameter(vertexProgram, "modelViewProj" );
  vertexSphereCenter = cgGetNamedParameter(vertexProgram, "sphereCenter");
  vertexSphereRadius = cgGetNamedParameter(vertexProgram, "sphereRadius");
  vertexAnimationFraction = cgGetNamedParameter(vertexProgram, "animationFraction");
  geometryModelViewProj = cgGetNamedParameter(geometryProgram, "modelViewProj" );
  geometryAnimationFraction = cgGetNamedParameter(geometryProgram, "animationFraction");
  fragmentBlueHue = cgGetNamedParameter(fragmentProgram, "blueHue" );
  fragmentCenter = cgGetNamedParameter(fragmentProgram, "center" );
  fragmentEye = cgGetNamedParameter(fragmentProgram, "eye" );
  fragmentAABBC0 = cgGetNamedParameter(fragmentProgram, "C0" );
  fragmentAABBC1 = cgGetNamedParameter(fragmentProgram, "C1" );

  return true;

}

void ShaderOneInterface::PerFrameInit() {

	//set parameters
	cgGLSetStateMatrixParameter(
    vertexModelViewProj, 
		CG_GL_MODELVIEW_PROJECTION_MATRIX, 
    CG_GL_MATRIX_IDENTITY);

  cgGLSetStateMatrixParameter(
    geometryModelViewProj, 
		CG_GL_MODELVIEW_PROJECTION_MATRIX, 
    CG_GL_MATRIX_IDENTITY);

  AABB aabb = scene->tms[0]->ComputeAABB();
  float radius = (aabb.corners[0]-aabb.corners[1]).Length() / 4.0f;
  cgGLSetParameter1f(vertexSphereRadius, radius);
  V3 center = scene->tms[0]->GetCenter();
  cgGLSetParameter3f(vertexSphereCenter, center[0], center[1], center[2]);
  cgGLSetParameter1f(vertexAnimationFraction, scene->animationFraction);

  cgGLSetParameter1f(geometryAnimationFraction, scene->animationFraction);

  float valueUsedToSetFragmentShaderParameter = 0.95f;
  cgGLSetParameter1f(fragmentBlueHue, valueUsedToSetFragmentShaderParameter);
  center = V3(1.0f, 0.5, 0.0f);
  cgGLSetParameter3f(fragmentCenter, center[0], center[1], center[2]);
  V3 eye = scene->ppc->C;
  cgGLSetParameter3f(fragmentEye, eye[0], eye[1], eye[2]);
  V3 C0 = aabb.corners[0];
  cgGLSetParameter3f(fragmentAABBC0, C0[0], C0[1], C0[2]);
  V3 C1 = aabb.corners[1];
  cgGLSetParameter3f(fragmentAABBC1, C1[0], C1[1], C1[2]);

}

void ShaderOneInterface::PerFrameDisable() {
}


void ShaderOneInterface::BindPrograms() {

#ifdef GEOM_SHADER
  cgGLBindProgram(geometryProgram);
#endif
  cgGLBindProgram(vertexProgram);
  cgGLBindProgram(fragmentProgram);

}

void CGInterface::DisableProfiles() {

  cgGLDisableProfile(vertexCGprofile);
#ifdef GEOM_SHADER
  cgGLDisableProfile(geometryCGprofile);
#endif
  cgGLDisableProfile(pixelCGprofile);

}

void CGInterface::EnableProfiles() {

  cgGLEnableProfile(vertexCGprofile);
#ifdef GEOM_SHADER
  cgGLEnableProfile(geometryCGprofile);
#endif
  cgGLEnableProfile(pixelCGprofile);

}

