#ifndef VRRENDERSTATE_H
#define VRRENDERSTATE_H

/** This data structure gets passed to the application programmer's draw callback function.  It holds state info 
    for the current OpenGL context and the current "VR context".  We think of this as analogous to the special and
    built-in variables in GLSL (e.g., gl_ModelViewProjectionMatrix, gl_VertexID).  There are some data associated 
    with the current drawing state that programmers will want an easy way to query during their application's
    draw() routine.
*/
class VRRenderState {
public:

  /*VRProjectionMethod::ProjectionType projectionType;
  float *projectionMatrix;
  float *headMatrix;
  float *leftEyeMatrix;
  float *rightEyeMatrix;*/
};

#endif

