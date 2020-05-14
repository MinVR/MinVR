
#include "../include/GfxMgr.H"
#include "../include/ConfigVal.H"
#include "../include/StringUtils.H"


namespace MinVR {

using namespace G3D;

 int GfxMgr::_nextPoseCallbackID = 0;
 int GfxMgr::_nextOneTimePoseCallbackID = 0;
 int GfxMgr::_nextDrawCallbackID = 0;

GfxMgr::GfxMgr(G3D::RenderDevice *renderDevice, ProjectionVRCameraRef camera)
{
  _renderDevice = renderDevice;
  _camera = camera;
  _roomToVirtualScale = 1.0;
  _lighting = Lighting::create();
  _skyLightingParams = SkyParameters(G3D::toSeconds(10, 00, 00, AM));
}

GfxMgr::~GfxMgr()
{
}



void
GfxMgr::poseFrame()
{
  double scale = 1.0/getRoomToVirtualSpaceScale();
  Matrix3 scaleMat(scale,0,0, 0,scale,0, 0,0,scale);
  CoordinateFrame virtualToRoomSpace = getRoomToVirtualSpaceFrame().inverse() * 
      CoordinateFrame(scaleMat,Vector3::zero());

  _posedModels.clear();

  if (_oneTimePoseCallbacks.size()) {
    Array<int> ids1 = _oneTimePoseCallbacks.getKeys();
    for (int i=0;i<ids1.size();i++) {
      _oneTimePoseCallbacks[ids1[i]]->exec(_posedModels, virtualToRoomSpace);
      delete _oneTimePoseCallbacks[ids1[i]];
    }
    _oneTimePoseCallbacks.clear();
  }  

  Array<int> ids2 = _poseCallbacks.getKeys();
  for (int i=0;i<ids2.size();i++) {
    _poseCallbacks[ids2[i]]->exec(_posedModels, virtualToRoomSpace);
  }
}


void
GfxMgr::drawFrame(Vector3 lookVec)
{
  debugAssertGLOk();

  // TODO: check to make sure RESCALE_NORMAL is supported before using it - it isn't on
  // my tablet
  //glEnable(GL_RESCALE_NORMAL);

  glEnable(GL_NORMALIZE);

  TextureRef bgTex = getTexture("BackgroundImage");
  if (bgTex.notNull()) {
    _renderDevice->pushState();
    _renderDevice->setDepthWrite(false);
    _renderDevice->disableLighting();
    _renderDevice->setTexture(0, bgTex);
    _renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
    _renderDevice->push2D();
    Rect2D r(Rect2D::xywh(0, 0, _renderDevice->width(), _renderDevice->height()));
    double scale = ConfigVal("BackgroundImageRepeat", 1.0);
    Draw::rect2D(r, _renderDevice, Color3::white(), Rect2D::xywh(0, 0, scale, scale));
    _renderDevice->pop2D();
    _renderDevice->popState();
  }


  LightingRef lighting = _lighting;
  SkyParameters skyParams = _skyLightingParams;

  if (_sky.notNull()) {
    _renderDevice->pushState();
    _renderDevice->setAmbientLightColor(Color3::white());
    _sky->render(_renderDevice, skyParams);
    _renderDevice->popState();
  }

  // Rendering of PosedModels via typical G3D style
  // TODO: Add shadowed rendering
  if (_posedModels.size()) {
    _renderDevice->pushState();
    _renderDevice->setAmbientLightColor(Color3(0.25, 0.25, 0.25));

	for (int m = 0; m < _posedModels.size(); ++m) {
      _posedModels[m]->renderNonShadowed(_renderDevice, lighting);
    }

	/*
    // TODO: This lookVec isn't always correct
    Array<Surface::Ref> opaque, transparent;
    Surface::sortFrontToBack(_posedModels, lookVec);

    debugAssertGLOk();
    // Opaque unshadowed
    for (int m = 0; m < opaque.size(); ++m) {
      opaque[m]->renderNonShadowed(_renderDevice, lighting);
    }

    debugAssertGLOk();
    // Transparent unshadowed
    for (int m = 0; m < transparent.size(); ++m) {
      transparent[m]->renderNonShadowed(_renderDevice, lighting);
    }

    debugAssertGLOk();
	*/
    _renderDevice->popState();
  }



  Array<int> ids = _drawCallbacks.getKeys();
  if (ids.size()) {
    _renderDevice->pushState();
 
    // Setup lights based on Lighting parameters
    _renderDevice->enableLighting();
    // Ambient
    _renderDevice->setAmbientLightColor(lighting->ambientTop);
    if (lighting->ambientBottom != lighting->ambientTop) {
      _renderDevice->setLight(0, GLight::directional(-Vector3::unitY(), 
            lighting->ambientBottom - lighting->ambientTop, false)); 
    }
          
    // Lights
    for (int L = 0; L < iMin(8, _lighting->lightArray.size()); ++L) {
      _renderDevice->setLight(L + 1, lighting->lightArray[L]);
    }

    // Compute virtualToRoom transformation
    double scale = 1.0/getRoomToVirtualSpaceScale();
    Matrix3 scaleMat(scale,0,0, 0,scale,0, 0,0,scale);
    CoordinateFrame virtualToRoomSpace = getRoomToVirtualSpaceFrame().inverse() * 
        CoordinateFrame(scaleMat,Vector3::zero());

    // Call draw callbacks
    for (int i=0;i<ids.size();i++) {
      _drawCallbacks[ids[i]]->exec(_renderDevice, virtualToRoomSpace);
    }

    _renderDevice->popState();
  }

  
  if (_sky.notNull()) {
    _renderDevice->pushState();
    _renderDevice->setAmbientLightColor(Color3::white());
    _sky->renderLensFlare(_renderDevice, skyParams);
    _renderDevice->popState();
  }

  debugAssertGLOk();
}

float 
GfxMgr::secPerFrame()
{
  return 1.0/_renderDevice->stats().smoothFrameRate;
}


void
GfxMgr::drawStats(bool showFrameRate, bool showTriRate, bool showTrisPerFrame)
{
  std::string msg("");
  if (showFrameRate) {
    msg = format("%3d fps", iRound(_renderDevice->stats().frameRate));
  }
  if (showTriRate) {
    msg = msg + format("%20d tris/s", iRound(_renderDevice->stats().triangleRate));
  }
  if (showTrisPerFrame) {
    msg = msg + format("%20d tris/f", iRound(_renderDevice->stats().triangleRate * (1/_renderDevice->stats().frameRate)));
  }

  _renderDevice->push2D();
  getDefaultFont()->draw2D(_renderDevice, msg, Vector2(5, 5), 12, Color3(0.61,0.72,0.92), Color4::clear() );
  _renderDevice->pop2D();
}



/***  Begin Texture Management Routines   ***/

TextureRef
GfxMgr::getTexture(const std::string &keyName)
{
  if (_texMap.containsKey(keyName))
    return _texMap[keyName];
  else
    return NULL;
}

Array<TextureRef> 
GfxMgr::getTextures(const Array<std::string> &keyNames)
{
  Array<TextureRef> ta;
  for (int i=0;i<keyNames.size();i++) {
    ta.append(getTexture(keyNames[i]));
  }
  return ta;
}

void
GfxMgr::setTextureEntry(const std::string &keyName, TextureRef tex)
{
  _texMap.set(keyName, tex);
}

void
GfxMgr::removeTextureEntry(const std::string &keyName)
{
  _texMap.remove(keyName);
}


std::string
GfxMgr::lookupTextureKeyName(TextureRef tex)
{
  Array<std::string> keys = _texMap.getKeys();
  for (int i=0;i<keys.size();i++)
    if (_texMap[keys[i]] == tex)
      return keys[i];
  return std::string("");
}

Array<std::string>
GfxMgr::getAllTextureKeys()
{
  return _texMap.getKeys();
}

void
GfxMgr::loadTexturesFromConfigVal(const std::string &configName, Log*log)
{
  Array<std::string> newTexKeys;

  std::string in = ConfigVal(configName,"");

  while (in.size()) {
    std::string               filename;
    std::string               alphafilename;
    std::string               keyname;
    std::string               format("AUTO");
    std::string               token;
    WrapMode                  wrap   = WrapMode::TILE;
    Texture::InterpolateMode  interp = Texture::TRILINEAR_MIPMAP;
    Texture::Dimension        dim    = Texture::DIM_2D_NPOT;
    double                    bright = 1.0;

    int semicolonpos = in.find(";");
    alwaysAssertM(semicolonpos >= 0, "Expected a ; to signal the end of "
      "each texture in the LoadTextures ConfigVal.");

    int commapos = in.find(",");
    bool hasalpha = ((commapos >= 0) && (commapos <= semicolonpos));

    if (!popNextToken(in, filename, true))
      alwaysAssertM(false, "Expected filename while loading texture from ConfigVal");

    if (hasalpha) {
      // remove the , separating the filename from the alphafilename
      in = in.substr(1);
      if (!popNextToken(in, alphafilename, true))
        alwaysAssertM(false, "TexMgr expected filename for alpha channel while loading texture from ConfigVal");
      // assume format of RGBA8 if alpha is specified, AUTO doesn't seem to work by default.
      format = std::string("RGBA8");
    }

    if (!popNextToken(in, keyname, true))
      alwaysAssertM(false, "TexMgr expected keyname while loading texture from ConfigVal");

    // check for optional param TextureFormat
    if (popNextToken(in, format, true)) {
      // check for optional param WrapMode
      if (popNextToken(in, token, true)) {
        if (token == "TILE")
          wrap = WrapMode::TILE;
        else if (token == "CLAMP")
          wrap = WrapMode::CLAMP;
        else if (token == "IGNORE")
          wrap = WrapMode::IGNORE;
        else if (token == "ZERO")
          wrap = WrapMode::ZERO;
        else if (token == "ERROR")
          wrap = WrapMode::ERROR;
        else 
          alwaysAssertM(false, "Invalid wrap mode while loading texture from ConfigVal: " + token);

        // check for optional param InterpolateMode
        if (popNextToken(in, token, true)) {
	  if (token == "TRILINEAR_MIPMAP")
	    interp = Texture::TRILINEAR_MIPMAP;
	  else if (token == "BILINEAR_MIPMAP")
	    interp = Texture::BILINEAR_MIPMAP;
	  else if (token == "NEAREST_MIPMAP")
	    interp = Texture::NEAREST_MIPMAP;
	  else if (token == "BILINEAR_NO_MIPMAP")
	    interp = Texture::BILINEAR_NO_MIPMAP;
	  else if (token == "NEAREST_NO_MIPMAP")
	    interp = Texture::NEAREST_NO_MIPMAP;
	  else
            alwaysAssertM(false, "Invalid interpolation mode for texture while loading from ConfigVal: " + token);

          // check for optional param Dimension
          if (popNextToken(in, token, true)) {
            if (token == "DIM_2D")
              dim = Texture::DIM_2D;
            else if (token == "DIM_2D_RECT")
              dim = Texture::DIM_2D_RECT;
            else if (token == "DIM_CUBE_MAP")
              dim = Texture::DIM_CUBE_MAP;
            else
              alwaysAssertM(false, "Invalid dimension while loading texture from ConfigVal: " + token);

            // check for optional param Brightness
            if (popNextToken(in, token, true)) {
              std::istringstream istr(token.c_str());
              istr >> bright;
            } // end brightness

          } // end dimension
        } // end interpolate mode
      } // end wrap mode
    } // end format


    filename = decygifyPath(replaceEnvVars(filename));
    alphafilename = decygifyPath(replaceEnvVars(alphafilename));

    if (!FileSystem::exists(filename))
      alwaysAssertM(false, "Texture file does not exist: " + filename);
    if ((hasalpha) && (!FileSystem::exists(alphafilename)))
      alwaysAssertM(false, "Texture file does not exist: " + alphafilename);

    std::string msg;
    if (hasalpha) {
      msg = "Loading texture " + filename + " with alpha texture " + alphafilename + " and keyname " + keyname;
    }
    else {
      msg = "Loading texture " + filename + " with keyname " + keyname;
    }
    if (log) {
      log->println(msg);
    }
    else {
      cout << msg << endl;
    }


    // Added by dlemmerm 3/18/05
    // G3D nicely throws an exception if it can't load one of these. We should handle it.
    TextureRef tex = NULL;
    Texture::Settings settings;
    settings.interpolateMode = interp;
    settings.wrapMode = wrap;
    Texture::Preprocess preProcess;
    preProcess.modulate = Color4(bright,bright,bright);

    try { 
      if (hasalpha) {
	if (format == "L8")
	  tex = Texture::fromTwoFiles(filename,alphafilename,TextureFormat::L8(),dim, settings);
	else if (format == "A8")
	  tex = Texture::fromTwoFiles(filename,alphafilename,TextureFormat::A8(),dim, settings);
	else if (format == "LA8")
	  tex = Texture::fromTwoFiles(filename,alphafilename,TextureFormat::LA8(),dim, settings);
	else if (format == "RGB5")
	  tex = Texture::fromTwoFiles(filename,alphafilename,TextureFormat::RGB5(),dim, settings);
	else if (format == "RGB5A1") 
	  tex = Texture::fromTwoFiles(filename,alphafilename,TextureFormat::RGB5A1(),dim, settings);
	else if (format == "RGB8")
	  tex = Texture::fromTwoFiles(filename,alphafilename,TextureFormat::RGB8(),dim, settings);
	else if (format == "RGBA8")
	  tex = Texture::fromTwoFiles(filename,alphafilename,TextureFormat::RGBA8(),dim, settings);
	else if (format == "RGB_DXT1") 
	  tex = Texture::fromTwoFiles(filename,alphafilename,TextureFormat::RGB_DXT1(),dim, settings);
	else if (format == "RGBA_DXT1")
	  tex = Texture::fromTwoFiles(filename,alphafilename,TextureFormat::RGBA_DXT1(),dim, settings);
	else if (format == "RGBA_DXT3")
	  tex = Texture::fromTwoFiles(filename,alphafilename,TextureFormat::RGBA_DXT3(),dim, settings);
	else if (format == "RGBA_DXT5")
	  tex = Texture::fromTwoFiles(filename,alphafilename,TextureFormat::RGBA_DXT5(),dim, settings);
	else if (format == "AUTO")
	  tex = Texture::fromTwoFiles(filename,alphafilename,TextureFormat::AUTO(),dim, settings);
	else if (format == "") // format not specified, assume AUTO
	  tex = Texture::fromTwoFiles(filename,alphafilename,TextureFormat::AUTO(),dim, settings);
	else
	  alwaysAssertM(false, "Invalid texture format while loading texture from ConfigVal: " + format);
      }
      else {
	if (format == "L8")
	  tex = Texture::fromFile(filename,TextureFormat::L8(),dim, settings, preProcess);
	else if (format == "A8")
	  tex = Texture::fromFile(filename,TextureFormat::A8(),dim, settings, preProcess);
	else if (format == "LA8")
	  tex = Texture::fromFile(filename,TextureFormat::LA8(),dim, settings, preProcess);
	else if (format == "RGB5")
	  tex = Texture::fromFile(filename,TextureFormat::RGB5(),dim, settings, preProcess);
	else if (format == "RGB5A1") 
	  tex = Texture::fromFile(filename,TextureFormat::RGB5A1(),dim, settings, preProcess);
	else if (format == "RGB8")
	  tex = Texture::fromFile(filename,TextureFormat::RGB8(),dim, settings, preProcess);
	else if (format == "RGBA8")
	  tex = Texture::fromFile(filename,TextureFormat::RGBA8(),dim, settings, preProcess);
	else if (format == "RGB_DXT1") 
	  tex = Texture::fromFile(filename,TextureFormat::RGB_DXT1(),dim, settings, preProcess);
	else if (format == "RGBA_DXT1")
	  tex = Texture::fromFile(filename,TextureFormat::RGBA_DXT1(),dim, settings, preProcess);
	else if (format == "RGBA_DXT3")
	  tex = Texture::fromFile(filename,TextureFormat::RGBA_DXT3(),dim, settings, preProcess);
	else if (format == "RGBA_DXT5")
	  tex = Texture::fromFile(filename,TextureFormat::RGBA_DXT5(),dim, settings, preProcess);
	else if (format == "AUTO")
	  tex = Texture::fromFile(filename,TextureFormat::AUTO(),dim, settings, preProcess);
	else if (format == "") // format not specified, assume AUTO
	  tex = Texture::fromFile(filename,TextureFormat::AUTO(),dim, settings, preProcess);
	else
	  alwaysAssertM(false, "Invalid texture format while loading texture from ConfigVal: " + format);
      }    
    }
    catch ( G3D::GImage::Error& texErr ) {

      cerr << "GfxMgr texture loading error. " << endl
        << "  File:\t" << texErr.filename << endl
        << "  Reason:\t" << texErr.reason << endl;

      tex = NULL;
    }

    alwaysAssertM(tex.notNull(), "Problem loading texture: " + filename);

    _texMap.set(keyname, tex);
    newTexKeys.append(keyname);
  }
}








/***  Begin Coordiante System Routines   ***/


CoordinateFrame
GfxMgr::getRoomToVirtualSpaceFrame()
{
  return _roomToVirtual;
}

double
GfxMgr::getRoomToVirtualSpaceScale()
{
  return _roomToVirtualScale;
}

void
GfxMgr::setRoomToVirtualSpaceFrame(const CoordinateFrame &roomToVirtual)
{
  _roomToVirtual = roomToVirtual;
}

void
GfxMgr::setRoomToVirtualSpaceScale(const double scale)
{
  _roomToVirtualScale = scale;
}


Vector3
GfxMgr::roomPointToVirtualSpace(const Vector3 &v)
{
  Vector3 v2 = _roomToVirtual.pointToWorldSpace(v);
  return _roomToVirtualScale * v2;
}

Vector3
GfxMgr::roomVectorToVirtualSpace(const Vector3 &v)
{
  Vector3 v2 = _roomToVirtual.vectorToWorldSpace(v);
  return _roomToVirtualScale * v2;
}

Vector3
GfxMgr::roomNormalToVirtualSpace(const Vector3 &v)
{
  Vector3 v2 = _roomToVirtual.normalToWorldSpace(v);
  return _roomToVirtualScale * v2;
}

double
GfxMgr::roomDistanceToVirtualSpace(const double &d)
{
  return roomVectorToVirtualSpace(Vector3(d,0,0)).length();
}


Vector3
GfxMgr::virtualPointToRoomSpace(const Vector3 &v)
{
  Vector3 v2 = 1.0/_roomToVirtualScale * v;
  return _roomToVirtual.pointToObjectSpace(v2);
}

Vector3
GfxMgr::virtualVectorToRoomSpace(const Vector3 &v)
{
  Vector3 v2 = 1.0/_roomToVirtualScale * v;
  return _roomToVirtual.vectorToObjectSpace(v2);
}

Vector3
GfxMgr::virtualNormalToRoomSpace(const Vector3 &v)
{
  Vector3 v2 = 1.0/_roomToVirtualScale * v;
  return _roomToVirtual.normalToObjectSpace(v2);
}

double
GfxMgr::virtualDistanceToRoomSpace(const double &d)
{
  return virtualVectorToRoomSpace(Vector3(d,0,0)).length();
}


Ray
GfxMgr::roomToVirtualSpace(const Ray &r)
{
  Vector3 o = roomPointToVirtualSpace(r.origin());
  Vector3 d = roomVectorToVirtualSpace(r.direction());
  return Ray::fromOriginAndDirection(o, d.unit());
}

Ray
GfxMgr::virtualToRoomSpace(const Ray &r)
{
  Vector3 o = virtualPointToRoomSpace(r.origin());
  Vector3 d = virtualVectorToRoomSpace(r.direction());
  return Ray::fromOriginAndDirection(o, d.unit());
}


CoordinateFrame
GfxMgr::roomToVirtualSpace(const CoordinateFrame &c)
{
  CoordinateFrame cv = _roomToVirtual * c;
  cv.translation *= _roomToVirtualScale;
  return cv;
}
  
CoordinateFrame
GfxMgr::virtualToRoomSpace(const CoordinateFrame &c)
{
  CoordinateFrame cTmp = c;
  cTmp.translation /= _roomToVirtualScale;
  CoordinateFrame cr = _roomToVirtual.inverse() * cTmp;
  return cr;
}




Vector3
GfxMgr::screenPointToRoomSpaceFilmplane(const Vector2 &v)
{
  // Mouse coordinates are reported from -1 -> 1, normalize to 0 -> 1
  double x = (v[0] / 2.0) + 0.5;
  double y = (v[1] / 2.0) + 0.5;
  //cout << "v = " << v << endl;
  //cout << "x=" << x << endl;
  //cout << "y=" << y << endl;

  Vector3 xvec = _camera->tile.topRight - _camera->tile.topLeft;
  Vector3 yvec = _camera->tile.topRight - _camera->tile.botRight;
  //cout << "xvec=" << xvec << endl;
  //cout << "yvec=" << yvec << endl;
  
  Vector3 p = _camera->tile.botLeft + (x * xvec) + (y * yvec);
  //cout << "p=" << p << endl;
  return p;
}

Plane
GfxMgr::getFilmplane()
{
  return Plane(_camera->tile.botLeft, _camera->tile.botRight, _camera->tile.topRight);
}

Vector2
GfxMgr::screenPointToG3DCoords(const Vector2 &screenPt)
{
  double xg3d = (screenPt[0]+1.0)/2.0 * (double)_renderDevice->width();
  double yg3d = (double)_renderDevice->height() - ((screenPt[1]+1.0)/2.0*(double)_renderDevice->height());
  return Vector2(xg3d, yg3d);
}

Vector2
GfxMgr::pixelScreenPointToIS3DCoords(const Vector2 &pixelPt) 
{
  double xis3d = ((2.0*pixelPt[0])/(double)_renderDevice->width()) - 1.0;
  double yis3d = (2.0*(double)_renderDevice->height() - 2.0*pixelPt[1])/(double)_renderDevice->height() - 1.0;
  return Vector2(xis3d,yis3d);
}

Vector2
GfxMgr::roomSpacePointToScreenPoint(const Vector3 &roomPt)
{
  Ray r = Ray::fromOriginAndDirection(roomPt, (_camera->getCameraPos()-roomPt).unit());
  Vector3 filmPt = r.intersection(getFilmplane());
  if (!filmPt.isFinite()) {
    r = Ray::fromOriginAndDirection(roomPt, (roomPt - _camera->getCameraPos()).unit());
    filmPt = r.intersection(getFilmplane());
  }
  Vector3 xdir = _camera->tile.botRight - _camera->tile.botLeft;
  Vector3 ydir = _camera->tile.topLeft - _camera->tile.botLeft; 
  Vector3 toPt = filmPt - _camera->tile.botLeft; 
  Vector2 screenPt = Vector2((xdir.dot(toPt) * 2.0/xdir.dot(xdir)) - 1.0,
                             (ydir.dot(toPt) * 2.0/ydir.dot(ydir)) - 1.0);
  return screenPt;
}

Vector3
GfxMgr::screenPointToRoomSpacePoint(const Vector2 &v, bool &hitGeometry, int whichBuffer)
{
  int xg3d = iRound((v[0]+1.0)/2.0 * (double)_renderDevice->width());
  int yg3d = _renderDevice->height() - iRound((v[1]+1.0)/2.0*(double)_renderDevice->height());
  glReadBuffer(whichBuffer);
  double zopengl = _renderDevice->getDepthBufferValue(xg3d,yg3d);
  double znear   = _camera->tile.nearClip;
  double zfar    = _camera->tile.farClip;
  double zdevice = znear*zfar/(zfar-zopengl*(zfar-znear));
  // TODO: This only works if the camera is oriented pointing down the -Z axis
  Vector3 cameraPos = _camera->getCameraPos();
  double z = cameraPos[2] + znear - zdevice;

  hitGeometry = (zdevice <= 0.95*zfar);
  return screenPointToRoomSpaceZEqualsPlane(v, z);
}

Vector3 
GfxMgr::screenPointToRoomSpaceXYPlane(const Vector2 &v)
{
  return screenPointToRoomSpaceZEqualsPlane(v, 0);
}

Vector3 
GfxMgr::screenPointToRoomSpaceZEqualsPlane(const Vector2 &v, double z)
{
  Vector3 fpPoint = screenPointToRoomSpaceFilmplane(v);
  Vector3 cameraPos = _camera->getCameraPos();
  Vector3 dir = fpPoint - cameraPos;
  //cout << "fp=" << fpPoint << endl;
  //cout << "dir=" << dir << endl;
  
  Vector3 xy_normal(0,0,-1);
  Plane xyPlane(xy_normal, Vector3(0,0,z));
  Ray r = Ray::fromOriginAndDirection(cameraPos, dir.unit());
  Vector3 intersect = r.intersection(xyPlane);
  if (intersect != Vector3::inf())
    return intersect;
  else {
    // try flipping the normal on the xyPlane
    Plane xyPlane2(-xy_normal, Vector3(0,0,z));
    intersect = r.intersection(xyPlane2);
    return intersect;
  }
}


GFontRef 
GfxMgr::getDefaultFont()
{
  if (_defaultFont.isNull()) {
    std::string ffile = ConfigVal("GfxMgr_Font", "$(G)/src/SFVRG3D/vrg3dsvn-latest/vrg3d/share/eurostyle.fnt", false);
    if (!FileSystem::exists(ffile))
      cout << "Error: Could not load font file: " + ffile << endl;
    else
      _defaultFont = GFont::fromFile(ffile);
  }
  return _defaultFont;
}

/*template <class T>
int GfxMgr::addPoseCallback(T *thisPtr, void (T::*method)(G3D::Array<G3D::Surface::Ref> &posedModels, const G3D::CoordinateFrame &virtualToRoomSpace)) {
  PoseMethodFunctor *f = new SpecificPoseMethodFunctor<T>(thisPtr, method);
  int id = _nextPoseCallbackID;
  _poseCallbacks.set(id, f);
  _nextPoseCallbackID++;
  return id;
}*/

} // end namespace
