/**
 This file is part of the MinVR Open Source Project, which is developed and
 maintained collaboratively by the University of Minnesota and Brown University.
 
 Copyright (c) 2016-2017 Regents of the University of Minnesota and Brown University.
 This software is distributed under the BSD-3 Clause license, which can be found
 at: http://github.minvr.org/blob/master/LICENSE.txt
 
 Original Author(s) of this File:
	Dan Keefe, 2017, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */


#ifndef UIEXAMPLE_H
#define UIEXAMPLE_H


#include <api/MinVR.h>
using namespace MinVR;

#include "QuickShapes.h"

#include <glm/glm.hpp>
#include <vector>

/** This example program is meant to illustrate how to create a MinVR program
    that includes an interesting 3D user interface.  Here, the user interface
    assumes that each hand holds a 6 DOF tracker with one button on it.  A
    3D cursor follows each hand -- the right hand cursor is a paintbrush, and
    the left hand is a cube.  When the right hand button is pressed and held
    then the brush begins to paint a trail of multi-colored spherical "3D paint
    blobs".  When the left hand button is pressed and held, this "grabs onto"
    the entire 3D painting so that the user can then translate and rotate
    the scene.  The interface is a super-simplified version of the CavePainting
    tool (Keefe et al., 2001) and recently seen in TiltBrush and similar tools.
 */
class UIExample : public VRApp {
public:
    
    /** The constructor passes argc, argv, and a MinVR config file on to VRApp.
     */
    UIExample(int argc, char** argv);
    virtual ~UIExample();

    
    /** USER INTERFACE CALLBACKS **/
    
    virtual void onAnalogChange(const VRAnalogEvent &state);
    
    virtual void onButtonDown(const VRButtonEvent &state);
    
    virtual void onButtonUp(const VRButtonEvent &state);
    
    virtual void onTrackerMove(const VRTrackerEvent &state);
    
    
    /** RENDERING CALLBACKS **/
    
    virtual void onRenderGraphicsScene(const VRGraphicsState& state);
    
    virtual void onRenderGraphicsContext(const VRGraphicsState& state);
    
    
private:

    // The names of events that will trigger right hand tracking, left hand
    // tracking, painting, and grabbing.  These are different depending upon
    // whether you are running in a Cave, Vive, Oculus, zSpace, desktop, etc.
    std::string _rHandTrackerEvent;
    std::string _lHandTrackerEvent;
    std::string _paintOnEvent;
    std::string _paintOffEvent;
    std::string _grabOnEvent;
    std::string _grabOffEvent;
    
    // Transformation matrices for the left hand and right hand
    glm::mat4 _lhand;
    glm::mat4 _rhand;

    // UI state:  Painting is true when the right hand button is held down,
    // Grabbing is true when the left hand button is held down.
    bool _grabbing;
    bool _painting;

    // Transforms paint blobs to "room" space (the fixed physical space of
    // the room as defined by the coordinate system of the VR trackers).
    // Initially _paintingToRoom equals the identity matrix, but when the user
    // grabs and reframes the painting using the left hand, this matrix is
    // updated based on that left hand movement so that the user can rotate or
    // move the painting around within the phyiscal "room".
    glm::mat4 _paintingToRoom;

    class PaintBlob {
    public:
        glm::mat4 trans;
        float rad;
        float color[3];
    };
    std::vector<PaintBlob> _paintBlobs;

    // Could be replaced with much fancier graphics... just a quick way to draw
    // a few 3D shapes using OpenGL VBOs and shaders.
    QuickShapes *_quickShapes;
};


#endif
