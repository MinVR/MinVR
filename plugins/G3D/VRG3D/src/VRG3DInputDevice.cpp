#include "../include/VRG3DInputDevice.h"
#include <config/VRDataQueue.h>
#include "../include/VRG3DWindowToolkit.h"
#include <GLG3D/RenderDevice.h>
#include "G3D/Array.h"
#include "GLG3D/GEvent.h"
#include "../include/VRG3DEvent.h"
#include "api/VRWindowResizeEvent.h"


namespace MinVR
{
  VRG3DInputDevice::VRG3DInputDevice()
  {

  }

  VRG3DInputDevice::~VRG3DInputDevice()
  {

  }

  void VRG3DInputDevice::appendNewInputEventsSinceLastCall(VRDataQueue* queue)
  {
    G3DWindow* gwindow = g3dRenderDevice->window();

    if (!gwindow)
    {
      return;
    }
    G3D::GEvent g3dEvent;

    G3D::Array<G3D::GEvent> gEvents;
    while (gwindow->pollEvent(g3dEvent))
    {
      gEvents.append(g3dEvent);
    }

    appendToMinVREvents(gEvents, queue);
    
    

    //gwindow->pollEvent(g3dEvent);
    
  }


  PLUGIN_API void VRG3DInputDevice::addWindow(G3DWindow* g3dWindow)
  {
    _windows.push_back(g3dWindow);
  }

  void VRG3DInputDevice::appendToMinVREvents(G3D::Array<G3D::GEvent> gEvents, VRDataQueue* minvrEventQueue)
  {
    bool gotMouseMotion = false;
    std::vector<float> mousePos;
    for (int i = 0; i < gEvents.size(); i++)
    {
      G3D::GEvent event = gEvents[i];
      switch (event.type)
      {
      case G3D::GEventType::QUIT:
        exit(0);
        break;

      case G3D::GEventType::VIDEO_RESIZE:
      {
        
        //_gEvents.append(new MinVR::VRG3DEvent("WindowResize", G3D::Vector2(event.resize.w, event.resize.h)));

        std::vector<float> size;
        size.push_back(event.resize.w);
        size.push_back(event.resize.h);

        double oldw = g3dRenderDevice->width();
        double oldh = g3dRenderDevice->height();

        std::string eventName = "WindowResize";
        VRDataIndex event = VRWindowResizeEvent::createValidDataIndex(eventName, size);
        minvrEventQueue->push(event.serialize());

        ///rd->notifyResize(event.resize.w, event.resize.h);
        //rd->resize(event.resize.w, event.resize.h);
        G3D::Rect2D full = G3D::Rect2D::xywh(0, 0, g3dRenderDevice->width()
          , g3dRenderDevice->height());
        g3dRenderDevice->setViewport(full);


        //if (adjustTileOnResizeEvent)
        //{
        //  double w = rd->width();
        //  double h = rd->height();

        //  /** recompute room coordinates of the tile display
        //      if width changes more than height, then width controls what appears
        //      to be zoom and height is adjusted so that what was in the center
        //      of the screen before remains in the center of the screen.  if
        //      height changes more, then we do the opposite.
        //  */

        //  // scale factors
        //  double scalew = w / oldw;
        //  double scaleh = h / oldh;

        //  if (fabs(scalew - 1.0) >= fabs(scaleh - 1.0))
        //  {
        //    double newh = scalew * oldh;
        //    double frach = h / newh;
        //    double newy = tile.topLeft[1] + frach * (tile.botLeft[1] - tile.topLeft[1]);
        //    double diff_y_div2 = (newy - tile.botLeft[1]) / 2.0;
        //    tile.topLeft[1] -= diff_y_div2;
        //    tile.topRight[1] -= diff_y_div2;
        //    tile.botLeft[1] = newy - diff_y_div2;
        //    tile.botRight[1] = newy - diff_y_div2;
        //  }
        //  else
        //  {
        //    double neww = scaleh * oldw;
        //    double fracw = w / neww;
        //    double newx = tile.topLeft[0] + fracw * (tile.topRight[0] - tile.topLeft[0]);
        //    double diff_x_div2 = (newx - tile.topRight[0]) / 2.0;
        //    tile.topLeft[0] -= diff_x_div2;
        //    tile.topRight[0] = newx - diff_x_div2;
        //    tile.botLeft[0] -= diff_x_div2;
        //    tile.botRight[0] = newx - diff_x_div2;
        //  }

        //  // recompute room2tile matrix
        //  Vector3 center = (tile.topLeft + tile.topRight + tile.botLeft + tile.botRight) / 4.0;
        //  Vector3 x = (tile.topRight - tile.topLeft).unit();
        //  Vector3 y = (tile.topLeft - tile.botLeft).unit();
        //  Vector3 z = x.cross(y).unit();
        //  Matrix3 rot(x[0], y[0], z[0], x[1], y[1], z[1], x[2], y[2], z[2]);
        //  CoordinateFrame tile2room(rot, center);
        //  tile.room2tile = tile2room.inverse();
        //}
      }

      break;
      case G3D::GEventType::MOUSE_MOTION:
        gotMouseMotion = true;
        
        mousePos.push_back(event.motion.x);
        mousePos.push_back(event.motion.y);

        double fracx = (double)event.motion.x / (double)g3dRenderDevice->width();
        double fracy = (double)event.motion.y / (double)g3dRenderDevice->height();

        std::vector<float> vmouse;
        vmouse.push_back((fracx * 2.0) - 1.0);
        vmouse.push_back(-((fracy * 2.0) - 1.0));

        std::string eventName = "Mouse_Pointer";
        VRDataIndex event = VRCursorEvent::createValidDataIndex(eventName, mousePos, vmouse);
        minvrEventQueue->push(event.serialize());

        break;
      }
    }

   /* if (!gotMouseMotion) {
      // Events may not be coming through as events for this window
      // type, try polling
      int x, y;
      G3D::uint8 btns;
      g3dRenderDevice->window()->getRelativeMouseState(x, y, btns);
      if (Vector2(x, y) != mousePos) {
        double fracx = (double)x / (double)rd->width();
        double fracy = (double)y / (double)rd->height();
        Vector2 vmouse((fracx * 2.0) - 1.0, -((fracy * 2.0) - 1.0));
        events.append(new Event("Mouse_Pointer", vmouse));
        mousePos = vmouse;
      }
    }*/
  }

}