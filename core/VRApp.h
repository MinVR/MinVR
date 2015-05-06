#ifndef VRAPP_H
#define VRAPP_H

/** VRApp:
    An as-simple-as-possible public interface for VR Applications
 */
class VRApp {
public:

  virtual void handleUserInput(const std::vector<VREvent*> &inputEvents) {}
  virtual void updateStateBeforeDrawing() {}
  virtual void drawGraphics() {}
  virtual bool readyToShutdown() { return false; }

};

#endif

