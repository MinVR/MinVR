/** 
This file is part of the MinVR Open Source Project, which is developed and 
maintained collaboratively by the University of Minnesota and Brown University.

Copyright (c) 2016 Regents of the University of Minnesota and Brown University.
This software is distributed under the BSD-3 Clause license, which can be found
at: MinVR/LICENSE.txt.

Original Author(s) of this File: 
	Dan Orban, 2016, University of Minnesota (Originally VRRenderHelper.h/cpp)
	
Author(s) of Significant Updates/Modifications to the File:
	Dan Keefe, 2016, University of Minnesota (Use only built-in types, comment, 
	hide implementation details, put advanced functions in a separate class.) 
*/

#ifndef VRGRAPHICSSTATE_H_
#define VRGRAPHICSSTATE_H_

#include "config/VRDataIndex.h"

namespace MinVR {

/** The VRGraphicsState class is used to convey state information to users of MinVR
    within the two important rendering callbacks that users define:  
    VRMain::onRenderGraphicsContext(GraphicsState) and VRMain::onRenderGraphics(GraphicsState).
    Advanced users can refer to VRGraphicsStateAdvanced for more complex needs, 
    including access to additional advanced information about the current window, 
    viewport, graphics card, etc.  All users are encouraged to access the most 
    frequently used GraphicsState data (e.g., Projection Matrix) using the 
    corresponding member functions defined in the VRGraphicsState class; these are 
    guaranteed to return good defaults.
*/
class VRGraphicsState {
public:
	VRGraphicsState(VRDataIndex& state);
	~VRGraphicsState();

	/** Returns a pointer to a 16 element array that stores the current projection
	    matrix, calculated by MinVR taking into account the current display geometry
	    head tracking data, and stereo rendering state.  The pointer is valid as long
	    as the GraphicsState object is valid.  If the MinVR DisplayGraph does not
	    contain a node that sets the projection matrix then the identify matrix is
	    returned following the same convention as OpenGL -- this is the same as
	    glOrtho(-1, 1, -1, 1, 1, -1).
	*/
	const float * getProjectionMatrix() const;


	/** Returns a pointer to a 16 element array that stores the current view
	    matrix, calculated by MinVR taking into account the current display geometry
	    and head tracking data.  The pointer is valid as long as the GraphicsState
	    object is valid.  If the MinVR DisplayGraph does not contain a node that
	    sets the view matrix then the identify matrix is returned.
	*/
	const float * getViewMatrix() const;

	/** Returns a pointer to a 3 element array that stores the current camera
		position.  This takes stereo rendering and head tracking into account;
		in other words, if it is the left eye that is currently being rendered,
		then the (x,y,z) location of the left eye will be returned, as calculated
		by MinVR based on head tracking data and the current inter-ocular-distance.
		The same camera position can also be extracted from the view matrix - this
		function is provided as a convenience since users frequently need access
		to the current camera position.  The pointer is valid as long as the
		GraphicsState object exists.
	*/
	const float* getCameraPos() const;


	/**
	 * Returns true if the render callback is the first render call to the context, otherwise
	 * it returns false.
	 */
	bool isInitialRenderCall() const;

private:

	// opaque reference to internal implementation state
	class VRGraphicsStateInternal;
	VRGraphicsStateInternal &_;
};


} /* namespace MinVR */


#endif /* VRGRAPHICSSTATE_H_ */
