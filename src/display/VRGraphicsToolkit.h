#ifndef VRGRAPHICSTOOLKIT_H
#define VRGRAPHICSTOOLKIT_H


#include <math/VRRect.h>


namespace MinVR {
  
  
/** Abstract base class for graphics toolkits that are implemented in plugins.  Examples
    are OpenGL, DirectX, Processing.org, Blender, etc.. 
 */
class VRGraphicsToolkit {
public:
	~VRGraphicsToolkit() {}

    virtual std::string getName() = 0;
  
	virtual void clearScreen() {
		std::cerr << "clearScreen() not enabled in this VRGraphicsToolkit." << std::endl;
	}

	enum VRDRAWBUFFER {
		VRDRAWBUFFER_BACK = 0,
		VRDRAWBUFFER_FRONT = 1,
		VRDRAWBUFFER_BACKLEFT = 2,
		VRDRAWBUFFER_BACKRIGHT = 3,
		VRDRAWBUFFER_FRONTLEFT = 4,
		VRDRAWBUFFER_FRONTRIGHT = 5
	};
	virtual void setDrawBuffer(VRDRAWBUFFER buffer) {
		std::cerr << "setDrawBuffer() not enabled in this VRGraphicsToolkit." << std::endl;
	}

	virtual void setViewport(VRRect rect) {
		std::cerr << "setViewport() not enabled in this VRGraphicsToolkit." << std::endl;
	}

	virtual void disableDrawingOnOddColumns() {
		std::cerr << "disableDrawingOnOddColumns() not enabled in this VRGraphicsToolkit." << std::endl;
	}

	virtual void disableDrawingOnEvenColumns() {
		std::cerr << "disableDrawingOnEvenColumns() not enabled in this VRGraphicsToolkit." << std::endl;
	}

	virtual void enableDrawingOnAllColumns() {
		std::cerr << "enableDrawingOnAllColumns() not enabled in this VRGraphicsToolkit." << std::endl;
	}

	virtual void flushGraphics() {
		std::cerr << "flushGraphics() not enabled in this VRGraphicsToolkit." << std::endl;
	}

	virtual void finishGraphics() {
		std::cerr << "finishGraphics() not enabled in this VRGraphicsToolkit." << std::endl;
	}

};

} // end namespace

#endif