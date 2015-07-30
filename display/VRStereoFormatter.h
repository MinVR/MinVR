#ifndef VRSTEREOFORMATTER_H
#define VRSTEREOFORMATTER_H


class VRMono : public VRStereoFormatter {
public:
  VRMono() {}
  virtual ~VRMono() {}

  virtual int getNumRenderingPasses() { return 1; }
  
  virtual void startRenderingPass(int passNum, VRRenderState &state) {
    _graphicsToolkit->clear();
    state.projectionType = MonoProjection;
  }

  virtual void endRenderingPass(int passNum, VRRenderState &state) {}

};


class VRQuadBufferedStereo : public VRStereoFormatter {
public:
  VRQuadBufferedStereo() {}
  virtual ~VRQuadBufferedStereo() {}

  virtual int getNumRenderingPasses() { return 2; }
  
  virtual void startRenderingPass(int passNum, VRRenderState &state) {
    if (passNum == 0) {
      _graphicsToolkit->setDrawBuffer(BACK_LEFT);
      _graphicsToolkit->clear();
      state.projectionType = LeftEyeProjection;
    }
    else {
      _graphicsToolkit->setDrawBuffer(BACK_RIGHT);
      _graphicsToolkit->clear();
      state.projectionType = RightEyeProjection;
    }
  }

  virtual void endRenderingPass(int passNum, VRRenderState &state) {}

};


class VRSideBySideStereo : public VRStereoFormatter {
public:
  VRSideBySideStereo() {}
  virtual ~VRSideBySideStereo() {}

  virtual int getNumRenderingPasses() { return 2; }
  
  virtual void startRenderingPass(int passNum, VRRenderState &state) {
    if (passNum == 0) {
      _graphicsToolkit->clear();
      _graphicsToolkit->setViewport(0,0,state.viewportWidth/2, state.viewportHeight/2);
      state.projectionType = LeftEyeProjection;
    }
    else {
      _graphicsToolkit->clear();
      _graphicsToolkit->setViewport(state.viewportWidth/2,0,state.viewportWidth/2, state.viewportHeight/2);
      state.projectionType = RightEyeProjection;
    }
  }

  virtual void endRenderingPass(int passNum, VRRenderState &state) {}

};


class VRColumnInterlacedStereo : public VRStereoFormatter {
public:
  VRColumnInterlacedStereo() {}
  virtual ~VRColumnInterlacedStereo() {}

  virtual int getNumRenderingPasses() { return 2; }
  
  virtual void startRenderingPass(int passNum, VRRenderState &state) {
    if (passNum == 0) {
      _graphicsToolkit->clear();
      _graphicsToolkit->enableColumnMask(0);
      state.projectionType = LeftEyeProjection;
    }
    else {
      _graphicsToolkit->clear();
      _graphicsToolkit->enableColumnMask(1);
      state.projectionType = RightEyeProjection;
    }
  }

  virtual void endRenderingPass(int passNum, VRRenderState &state) {
    if (passNum == 1) {
      _graphicsToolkit->disableColumnMask();
    }
  }

};


#endif

