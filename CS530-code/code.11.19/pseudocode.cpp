
InteractiveGraphicsPipeline

  UserInteractionChangesView
#if SW
    Update PPC // update the planar pinhole camera that models the view the user wants
#else
    TELL HARDWARE ABOUT THE VIEW FROM TO RENDER // HW command
#endif

#if SW
  ClearFramebuffer // both z and color channels
#else
  TELL HARDWARE TO CLEAR THE (HW) FRAMEBUFFER // HW command
#endif

  if (cubemap of distant geometry)
    ClearFrameBufferToDistantGeometry

  for (int tmi = 0; tmi < tmsN; tmi++) { // for all objects (triangle meshes)
#if SW
    for (int vi = 0; vi < tms[tmi]->vertsN; vi++) {
      ppc->Project(tms[tmi]->verts[vi], tms[tmi]->projVerts[vi]); // project all vertices
    }
    for (int tri = 0; tri < tms[tmi]->trisN; tri++) { // for all triangles
      eeqs = SetupEdgeEquations(tms[tmi], tri); // setup edge equations for current triangle
      SetupRasterizationParameterInterpolation(tms[tmi], tri);
      aabb = SetupBoundingBoxOfProjectedTriangle(tms[tmi], tri);
      // rasterize current triangle
      for (v = aabb.top to aabb.bottom) {
        for (u = aabb.left to aabb.right) {
          if (!IsPixelInsideProjectedTriangle(u, v, eeqs))
            continue; // failed edge equation test
          float currz = Interpolatez(u, v, zABC);
          if (IsPixelHidden(u, v, currz))
            continue; // failed z buffer test (i.e. curr triangle hidden at curr pixel by what was rendered before)
          UpdateZbuffer(u, v, currz);
          // compute color at current pixel
          if (texture) {
            (s, t) = ComputeTextureCoordinates(u, v);
            (r, g, b) = LookupTexture(s, t)
          }
          if (color interpolation)
            (r, g, b) = InterpolateRGB(u, v);
          if (shadows)
            compute shadows;
          if (projective texture mapping)
            (r, g, b) = LookUpProjectiveTexture(u, v);
          if (reflective object) {
            Computer reflected ray;
            Lookup reflected ray in cube map;
          }
          
          // set color of current pixel
          SetFrameBufferColor((r, g, b), u, v);

        }
      }
    }
  }
#else
  PASS THE TRIANGLE MESH TO THE HARDWARE, ALONG WITH TMESH PARAMETERS NEEDED FOR RENDERING
#endif




// interactive graphics pipeline overview
if SW or FX_HW or PG_HW // SW pipeline; fixed HW pipeline; programmable HW pipeline
  ppc = SetView // based on input from the user, transfer to HW
if SW or FX_HW or PG_HW
  ClearFrameBuffer // color, z, any other channels
for all triangle meshes tm // render all objects (i.e. t meshes)
if FX_HW
  Tell HW to render tm
if SW or PG_HW
  for all vertices v in t
if SW
    pv = ppc.Project(v) // project all vertices of the shared vertex t mesh
if PG_HW
    pv = (1)VertexShader(v) // "shader" == "program" == "function"
  for all triangles t(v0, v1, v2) in tm
if PG_HW
    processedTriangle = (2)GeometryShader(t)

    eeqs = SetupEdgeEquations(t, pv0, pv1, pv2)
    ABCz = SetupZInterp(t, pv0, pv1, pv2)
    ABCDEF0 = SetupRastParInterp(t, r00, r01, r02) // same for other rasterization parameters
    aabb = AABB(pv0, pv1, pv2)
    for all rows v of aabb
      for all rows u of aabb
        if OutsideTriangle(u, v, eeqs) continue
        currz = LinearInterp(ABCx, u, v)
        if Hidden(zb, u, v, currz) continue
        currRP0 = Interpolate(u, v, ABCDEF0) // compute value of RP0 at current pixel
        currRP1 = Interpolate(u, v, ABCDEF1) // compute value of RP1 at current pixel
if SW
        framebuffer(u, v) = ComputeColor(currRP0, currRP1, ...) // sets color at current pixel based on current triangle
if PG_HW
        framebuffer(u, v) = (3)PixelShader(currRP0, currRP1, ...) // compute color in pixel shader


BillboardReflectionFragmentShader(PerPixelData ppd, 
    uniform billboard0, uniform billboard1, uniform eye, 
    uniform texture)

1. Compute reflected ray direction r 
    1.1 normal = ppd.normal // comes in as input
    1.2 xyzAtPixel = ppd.xyzAtPixel // comes in as input
    1.3 eye = eye // passed in as uniform variable (same for all pixels)
    1.4 eyeVector = (eye-xyzAtPixel).UnitLength()
    1.5 r = Reflect(eyeVector, normal)
2. Current intersection CI is empty
3. for all billboards B
    3.1 I.Point = (r, xyzAtPixel) IntersectedWith B.Rectangle
    3.2. compute B texture coordinates (s, t) at I
    3.3. I.color = LookUp B.Texture at (s, t)
    3.4 if I.color is background continue
    3.5 if I is closer than CI then CI = I
return I.color

B00---------------B01
|    |y            |
|    |             |
|---I.P            |
| x                |
|                  |
B10---------------B11

s =x / (B01-B00).Length();
t =y / (B10-B00).Length();








