
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


