#ifndef VROPENGLGRAPHICSTOOLKIT_H
#define VROPENGLGRAPHICSTOOLKIT_H

#include <display/VRGraphicsToolkit.h>
#include <main/VRFactory.h>


namespace MinVR {

  
  
/** OpenGL Implementation for the MinVR VRGraphicsToolkit abstraction.
 */
class VROpenGLGraphicsToolkit : public VRGraphicsToolkit {
public:
	VROpenGLGraphicsToolkit();
	virtual ~VROpenGLGraphicsToolkit();

    std::string getName() { return "VROpenGLGraphicsToolkit"; }
  
	void clearScreen();
	void setDrawBuffer(VRDRAWBUFFER buffer);
	void setViewport(VRRect rect);
	void flushGraphics();
	void finishGraphics();
};

class VROpenGLGraphicsToolkitFactory : public VRGraphicsToolkitFactory {
public:
	VROpenGLGraphicsToolkitFactory():VRGraphicsToolkitFactory("VROpenGLGraphicsToolkit"){}

	VRGraphicsToolkit* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace);
};


} // end namespace

#endif
