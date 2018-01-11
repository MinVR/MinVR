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


#include "UIExample.h"
#include "QuickShapes.h"

#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <cctype>




UIExample::UIExample(int argc, char** argv) : VRApp(argc, argv),
    _grabbing(false), _painting(false),
    _paintingToRoom(glm::mat4(1.0))
{
    
    // This is an important idiom for programming with MinVR.  We want to write
    // our program so that it will work across multiple VR setups.  Each setup
    // will have some form of trackers and buttons, but the events they generate
    // will be named something different depending on whether we are running in
    // a Cave or Vive or in desktop mode.  So, here in the constructor, we check
    // the name of the current VR setup and then set the name of the event to
    // listen for accordingly.
    
    std::string name = getVRSetupName();
    if (name.find("IVLABCave") != string::npos) {
        // IV/LAB Cave mode
        _rHandTrackerEvent = "RedStylus_Move";
        _lHandTrackerEvent = "BlueStylus_Move";
        _paintOnEvent = "RedStylusFrontBtn_Down";
        _paintOffEvent = "RedStylusFrontBtn_Up";
        _grabOnEvent = "BlueStylusFrontBtn_Down";
        _grabOffEvent = "BlueStylusFrontBtn_Up";
    }
    else if ((name.find("Vive") != string::npos) || (name.find("rift") != string::npos)) {
        // HTC Vive / Oculus Rift mode
        _rHandTrackerEvent = "RightController_Move";
        _lHandTrackerEvent = "LeftController_Move";
        _paintOnEvent = "RightTriggerBtn_Down";
        _paintOffEvent = "RightTriggerBtn_Up";
        _grabOnEvent = "LeftTriggerBtn_Down";
        _grabOffEvent = "LeftTriggerBtn_Up";
    }
    else if (name.find("Desktop") != string::npos) {
        // Desktop mode
        _rHandTrackerEvent = "RHandTracker_Move";
        _lHandTrackerEvent = "LHandTracker_Move";
        _paintOnEvent = "MouseBtnLeft_Down";
        _paintOffEvent = "MouseBtnLeft_Up";
        _grabOnEvent = "KbdSpace_Down";
        _grabOffEvent = "KbdSpace_Up";
    
    }
    // ... additional VRSetups can be added here ...
    else {
        std::cout << "Unrecognized VR setup `" << getVRSetupName() <<
            "` -- using Desktop mode events." << std::endl;
        // Assume desktop mode
        _rHandTrackerEvent = "RHandTracker_Move";
        _lHandTrackerEvent = "LHandTracker_Move";
        _paintOnEvent = "MouseBtnLeft_Down";
        _paintOffEvent = "MouseBtnLeft_Up";
        _grabOnEvent = "KbdSpace_Down";
        _grabOffEvent = "KbdSpace_Up";
    }

    
    _quickShapes = new QuickShapes;

}

UIExample::~UIExample()
{
    delete _quickShapes;
}


void UIExample::onAnalogChange(const VRAnalogEvent &state) {
    // This routine is called for all Analog_Change events.  Check event->getName()
    // to see exactly which analog input has been changed, and then access the
    // new value with event->getValue().
    
}


void UIExample::onButtonDown(const VRButtonEvent &event) {
    // This routine is called for all Button_Down events.  Check event->getName()
    // to see exactly which button has been pressed down.

    if (event.getName() == _paintOnEvent) {
        _painting = true;
    }
    else if (event.getName() == _grabOnEvent) {
        _grabbing = true;
    }
}


void UIExample::onButtonUp(const VRButtonEvent &event) {
    // This routine is called for all Button_Up events.  Check event->getName()
    // to see exactly which button has been released.
    
    if (event.getName() == _paintOffEvent) {
        _painting = false;
    }
    else if (event.getName() == _grabOffEvent) {
        _grabbing = false;
    }
}


void UIExample::onTrackerMove(const VRTrackerEvent &event) {
    // This routine is called for all Tracker_Move events.  Check event->getName()
    // to see exactly which tracker has moved, and then access the tracker's new
    // 4x4 transformation matrix with event->getTransform().
    
    
    //std::cout << event.getName() << " "
    //  << event.getPos()[0] << " " << event.getPos()[1] << " " << event.getPos()[2] << std::endl;
    
    if (event.getName() == _rHandTrackerEvent) {
        _rhand = glm::make_mat4(event.getTransform());
        if (_painting) {
            // Transform the right hand transform into "painting space"
            glm::mat4 roomToPainting = glm::inverse(_paintingToRoom);
            glm::mat4 handInPaintingSpace = roomToPainting * _rhand;
            
            // Create a new blob to add to the painting
            PaintBlob pb;
            // set the blob's transform based on the current hand transform
            pb.trans = handInPaintingSpace;
            // jitter the blob's position a bit so that blobs appear to come out
            // of the brush at a random location along the brush's bristles.
            float jitter = 0.5*((GLfloat)rand()/(GLfloat)RAND_MAX) - 0.25;
            pb.trans[3].x += jitter * pb.trans[0].x;
            pb.trans[3].y += jitter * pb.trans[0].y;
            pb.trans[3].z += jitter * pb.trans[0].z;
            
            // give the blob a random radius
            pb.rad = 0.01 + 0.1*((GLfloat)rand()/(GLfloat)RAND_MAX);
            
            // assign a color based on the position of the brush within the room
            float tmp;
            pb.color[0] = std::modf(_rhand[3].x, &tmp);
            pb.color[1] = std::modf(_rhand[3].y, &tmp);
            pb.color[2] = std::modf(_rhand[3].z, &tmp);
            
            _paintBlobs.push_back(pb);
        }
    }
    else if (event.getName() == _lHandTrackerEvent) {
        glm::mat4 newHand = glm::make_mat4(event.getTransform());
        if (_grabbing) {
            // Update the paintingToRoom transform based upon the new transform
            // of the left hand relative to the last frame.
            _paintingToRoom = newHand * glm::inverse(_lhand) * _paintingToRoom;
        }
        _lhand = newHand;
    }
}


    
void UIExample::onRenderGraphicsContext(const VRGraphicsState &renderState) {
    // This routine is called once per graphics context at the start of the
    // rendering process.  So, this is the place to initialize textures,
    // load models, or do other operations that you only want to do once per
    // frame when in stereo mode.
    
    if (renderState.isInitialRenderCall()) {
        #ifndef __APPLE__
            glewExperimental = GL_TRUE;
            GLenum err = glewInit();
            if (GLEW_OK != err) {
                std::cout << "Error initializing GLEW." << std::endl;
            }
        #endif        
        
        glEnable(GL_DEPTH_TEST);
        glClearDepth(1.0f);
        glDepthFunc(GL_LEQUAL);
        glClearColor(0.2, 0.2, 0.2, 1);
    }
}


void UIExample::onRenderGraphicsScene(const VRGraphicsState &renderState) {
    // This routine is called once per eye.  This is the place to actually
    // draw the scene.
    
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // Draw a cursor for the left hand
    const GLfloat lcolor [] = {0.8, 0.5, 0.5};
    glm::mat4 scaleHandCursor = glm::mat4(1.0);
    scaleHandCursor[0].x = 0.25;
    scaleHandCursor[1].y = 0.25;
    scaleHandCursor[2].z = 0.25;
    glm::mat4 lHandScaled = _lhand * scaleHandCursor;
    _quickShapes->drawCube(glm::value_ptr(lHandScaled), renderState.getViewMatrix(), renderState.getProjectionMatrix(), lcolor);
    
    // Draw a cursor for the right hand
    const float rcolor [] = {0.5, 0.5, 0.8};
    _quickShapes->drawBrush(glm::value_ptr(_rhand), renderState.getViewMatrix(), renderState.getProjectionMatrix(), rcolor);
    
    // Draw the "painting"
    for (int i=0; i<_paintBlobs.size(); i++) {
        glm::mat4 S = glm::mat4(1.0);
        S[0].x = _paintBlobs[i].rad;;
        S[1].y = _paintBlobs[i].rad;;
        S[2].z = _paintBlobs[i].rad;;
        glm::mat4 M = _paintingToRoom * _paintBlobs[i].trans * S;
        _quickShapes->drawSphere(glm::value_ptr(M), renderState.getViewMatrix(), renderState.getProjectionMatrix(), _paintBlobs[i].color);
    }
}
