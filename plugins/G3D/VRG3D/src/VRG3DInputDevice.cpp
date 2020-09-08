#include "../include/VRG3DInputDevice.h"
#include <config/VRDataQueue.h>
#include "../include/VRG3DWindowToolkit.h"
#include <GLG3D/RenderDevice.h>
#include "G3D/Array.h"
#include "GLG3D/GEvent.h"
#include "../include/VRG3DEvent.h"
#include "api/VRWindowResizeEvent.h"

using namespace G3D;

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
      case GEventType::KEY_DOWN:
      {
        std::string keyname = getKeyName(event.key.keysym.sym, event.key.keysym.mod);
        VRDataIndex event = VRButtonEvent::createValidDataIndex("kbd_" + keyname + "_down", 1);
        minvrEventQueue->push(event.serialize());
      }

      break;

      case GEventType::KEY_UP:
      {
        std::string keyname = getKeyName(event.key.keysym.sym, event.key.keysym.mod);
        VRDataIndex event = VRButtonEvent::createValidDataIndex("kbd_"+keyname +"_up", 0);
        minvrEventQueue->push(event.serialize());

        
      }

      break;
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

  std::string
    VRG3DInputDevice::getKeyName(G3D::GKey::Value key, G3D::GKeyMod mod)
  {
    std::string name;

    switch (key) {
    case G3D::GKey::F1:
      name = std::string("F1");
      break;
    case G3D::GKey::F2:
      name = std::string("F2");
      break;
    case G3D::GKey::F3:
      name = std::string("F3");
      break;
    case G3D::GKey::F4:
      name = std::string("F4");
      break;
    case G3D::GKey::F5:
      name = std::string("F5");
      break;
    case G3D::GKey::F6:
      name = std::string("F6");
      break;
    case G3D::GKey::F7:
      name = std::string("F7");
      break;
    case G3D::GKey::F8:
      name = std::string("F8");
      break;
    case G3D::GKey::F9:
      name = std::string("F9");
      break;
    case G3D::GKey::F10:
      name = std::string("F10");
      break;
    case G3D::GKey::F11:
      name = std::string("F11");
      break;
    case G3D::GKey::F12:
      name = std::string("F12");
      break;
    case G3D::GKey::KP0:
      name = std::string("KP0");
      break;
    case G3D::GKey::KP1:
      name = std::string("KP1");
      break;
    case G3D::GKey::KP2:
      name = std::string("KP2");
      break;
    case G3D::GKey::KP3:
      name = std::string("KP3");
      break;
    case G3D::GKey::KP4:
      name = std::string("KP4");
      break;
    case G3D::GKey::KP5:
      name = std::string("KP5");
      break;
    case G3D::GKey::KP6:
      name = std::string("KP6");
      break;
    case G3D::GKey::KP7:
      name = std::string("KP7");
      break;
    case G3D::GKey::KP8:
      name = std::string("KP8");
      break;
    case G3D::GKey::KP9:
      name = std::string("KP9");
      break;
    case G3D::GKey::KP_PERIOD:
      name = std::string("KP_PERIOD");
      break;
    case G3D::GKey::KP_MULTIPLY:
      name = std::string("KP_MULTIPLY");
      break;
    case G3D::GKey::KP_MINUS:
      name = std::string("KP_MINUS");
      break;
    case G3D::GKey::KP_PLUS:
      name = std::string("KP_PLUS");
      break;
    case G3D::GKey::KP_ENTER:
      name = std::string("KP_ENTER");
      break;
    case G3D::GKey::KP_EQUALS:
      name = std::string("KP_EQUALS");
      break;
    case G3D::GKey::LEFT:
      name = std::string("LEFT");
      break;
    case G3D::GKey::RIGHT:
      name = std::string("RIGHT");
      break;
    case G3D::GKey::UP:
      name = std::string("UP");
      break;
    case G3D::GKey::DOWN:
      name = std::string("DOWN");
      break;
    case G3D::GKey::PAGEUP:
      name = std::string("PAGEUP");
      break;
    case G3D::GKey::PAGEDOWN:
      name = std::string("PAGEDOWN");
      break;
    case G3D::GKey::HOME:
      name = std::string("HOME");
      break;
    case G3D::GKey::END:
      name = std::string("END");
      break;
    case G3D::GKey::INSERT:
      name = std::string("INSERT");
      break;
    case G3D::GKey::BACKSPACE:
      name = std::string("BACKSPACE");
      break;
    case G3D::GKey::TAB:
      name = std::string("TAB");
      break;
    case G3D::GKey::RETURN:
      name = std::string("ENTER");
      break;
    case G3D::GKey::ESCAPE:
      name = std::string("ESC");
      break;
    case G3D::GKey::SPACE:
      name = std::string("SPACE");
      break;
    case 48:
      name = std::string("0");
      break;
    case 49:
      name = std::string("1");
      break;
    case 50:
      name = std::string("2");
      break;
    case 51:
      name = std::string("3");
      break;
    case 52:
      name = std::string("4");
      break;
    case 53:
      name = std::string("5");
      break;
    case 54:
      name = std::string("6");
      break;
    case 55:
      name = std::string("7");
      break;
    case 56:
      name = std::string("8");
      break;
    case 57:
      name = std::string("9");
      break;
    case 97:
      name = std::string("A");
      break;
    case 98:
      name = std::string("B");
      break;
    case 99:
      name = std::string("C");
      break;
    case 100:
      name = std::string("D");
      break;
    case 101:
      name = std::string("E");
      break;
    case 102:
      name = std::string("F");
      break;
    case 103:
      name = std::string("G");
      break;
    case 104:
      name = std::string("H");
      break;
    case 105:
      name = std::string("I");
      break;
    case 106:
      name = std::string("J");
      break;
    case 107:
      name = std::string("K");
      break;
    case 108:
      name = std::string("L");
      break;
    case 109:
      name = std::string("M");
      break;
    case 110:
      name = std::string("N");
      break;
    case 111:
      name = std::string("O");
      break;
    case 112:
      name = std::string("P");
      break;
    case 113:
      name = std::string("Q");
      break;
    case 114:
      name = std::string("R");
      break;
    case 115:
      name = std::string("S");
      break;
    case 116:
      name = std::string("T");
      break;
    case 117:
      name = std::string("U");
      break;
    case 118:
      name = std::string("V");
      break;
    case 119:
      name = std::string("W");
      break;
    case 120:
      name = std::string("X");
      break;
    case 121:
      name = std::string("Y");
      break;
    case 122:
      name = std::string("Z");
      break;
    case G3D::GKey::RSHIFT:
    case G3D::GKey::LSHIFT:
      name = std::string("SHIFT");
      return name;
      break;
    case G3D::GKey::RCTRL:
    case G3D::GKey::LCTRL:
      name = std::string("CTRL");
      return name;
      break;
    case G3D::GKey::RALT:
    case G3D::GKey::LALT:
      name = std::string("ALT");
      return name;
      break;
    case G3D::GKey::PERIOD:
      name = ".";
      break;
    case G3D::GKey::COMMA:
      name = ",";
      break;
    case G3D::GKey::LEFTBRACKET:
      name = "[";
      break;
    case G3D::GKey::RIGHTBRACKET:
      name = "]";
      break;
    default:
      cerr << "Unknown keypress: " << (int)key << endl;
      name = std::string("UNKNOWN");
      break;
    }

    if (mod & GKEYMOD_SHIFT)
      name = "SHIFT_" + name;
    if (mod & GKEYMOD_ALT)
      name = "ALT_" + name;
    if (mod & GKEYMOD_CTRL)
      name = "CTRL_" + name;
    if (mod & GKEYMOD_META)
      name = "META_" + name;

    return name;
  }
}