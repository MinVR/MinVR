
#include "../include/LoadingScreen.H"

namespace MinVR {

using namespace G3D;

void
LoadingScreen::renderAndSwapBuffers(MinVR::GfxMgrRef gfxMgr, double percentDone)
{
  // TODO: really check for stereo here..
  if (false) {
    gfxMgr->getRenderDevice()->beginFrame();
    glDrawBuffer(GL_BACK_LEFT);
    gfxMgr->getRenderDevice()->clear(true,true,true);
    render(gfxMgr, percentDone);
    gfxMgr->getRenderDevice()->clear(true,true,true);
    glDrawBuffer(GL_BACK_RIGHT);
    render(gfxMgr, percentDone);
    gfxMgr->getRenderDevice()->endFrame();
  }
  else {
    gfxMgr->getRenderDevice()->beginFrame();
    glDrawBuffer(GL_BACK);
    gfxMgr->getRenderDevice()->clear(true,true,true);
    render(gfxMgr, percentDone);
    gfxMgr->getRenderDevice()->endFrame();
  }
}

void
LoadingScreen::render(MinVR::GfxMgrRef gfxMgr, double percentDone)
{
  RenderDevice *rd = gfxMgr->getRenderDevice();
  Vector2 size = Vector2(250, 60);
  Vector2 pos  = Vector2(rd->width()/2.0,
                         rd->height()/2.0);
  GFontRef font = gfxMgr->getDefaultFont();

  Color4 outlineCol  = ConfigVal("LoadingScreenOutlineCol",    Color4(1.0,1.0,1.0,1.0),false);
  Color4 bgCol       = ConfigVal("LoadingScreenBackgroundCol", Color4(0.1,0.1,0.44,1.0),false);
  Color4 barCol      = ConfigVal("LoadingScreenBarCol",        Color4(0.61,0.72,0.92,1.0),false);
  Color4 titleCol    = ConfigVal("LoadingScreenTitleCol",      Color4(1.0,1.0,1.0,1.0),false);//Color4(0.61,0.72,0.92,1.0),false);
  Color4 percCol     = ConfigVal("LoadingScreenPercCol",       Color4(0.8,0.22,0.0,1.0),false);

  int perc = iRound(percentDone);
  if (perc > 100)
    perc = 100;
  
  Vector2 topl = Vector2(pos[0]-size[0]/2.0, pos[1]-size[1]/2.0);
  Vector2 topr = Vector2(pos[0]+size[0]/2.0, pos[1]-size[1]/2.0);
  Vector2 botl = Vector2(pos[0]-size[0]/2.0, pos[1]+size[1]/2.0);
  Vector2 botr = Vector2(pos[0]+size[0]/2.0, pos[1]+size[1]/2.0);

  Vector2 titleCtr = pos - Vector2(0, size[1] / 6.0);
  Vector2 barCtr   = pos + Vector2(0, size[1] / 6.0);

  rd->push2D();
  rd->setTexture(0,NULL);

  // outline
  rd->setColor(outlineCol);
  rd->beginPrimitive(PrimitiveType::QUADS);
  rd->sendVertex(topl);
  rd->sendVertex(botl);
  rd->sendVertex(botr);
  rd->sendVertex(topr);
  rd->endPrimitive();

  // background
  rd->setColor(bgCol);
  rd->beginPrimitive(PrimitiveType::QUADS);
  rd->sendVertex(Vector2(topl[0] + 2, topl[1] + 2));
  rd->sendVertex(Vector2(botl[0] + 2, botl[1] - 2));
  rd->sendVertex(Vector2(botr[0] - 2, botr[1] - 2));
  rd->sendVertex(Vector2(topr[0] - 2, topr[1] + 2));
  rd->endPrimitive();

  // progress bar
  float left  = barCtr[0] - (size[0]-20)/2.0;
  float right = barCtr[0] + (size[0]-20)/2.0;
  float maxwidth = right - left;
  float w = percentDone/100.0 * maxwidth;
  
  rd->setColor(outlineCol);
  rd->beginPrimitive(PrimitiveType::QUADS);
  rd->sendVertex(Vector2(left, barCtr[1]-size[1]/6.0));
  rd->sendVertex(Vector2(left, barCtr[1]+size[1]/6.0));
  rd->sendVertex(Vector2(right, barCtr[1]+size[1]/6.0));
  rd->sendVertex(Vector2(right, barCtr[1]-size[1]/6.0));
  rd->endPrimitive();

  rd->setColor(barCol);
  rd->beginPrimitive(PrimitiveType::QUADS);
  rd->sendVertex(Vector2(left, barCtr[1]-size[1]/6.0));
  rd->sendVertex(Vector2(left, barCtr[1]+size[1]/6.0));
  rd->sendVertex(Vector2(left + w, barCtr[1]+size[1]/6.0));
  rd->sendVertex(Vector2(left + w, barCtr[1]-size[1]/6.0));
  rd->endPrimitive();
  
  // title
  font->draw2D(rd, "Loading..", titleCtr, 12, titleCol, Color4::clear(),
		GFont::XALIGN_CENTER, GFont::YALIGN_BOTTOM);

  // percentage
  font->draw2D(rd, intToString(perc) + " %", barCtr, 12, percCol, Color4::clear(),
               GFont::XALIGN_CENTER, GFont::YALIGN_CENTER);
  rd->pop2D();
}



} // end namespace

