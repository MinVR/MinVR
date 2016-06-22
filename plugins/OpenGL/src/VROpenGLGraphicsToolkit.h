#ifndef VROPENGLGRAPHICSTOOLKIT_H
#define VROPENGLGRAPHICSTOOLKIT_H

#include <display/VRGraphicsToolkit.h>
#include <main/VRFactory.h>
#include <plugin/VRPlugin.h>

namespace MinVR {

  
  
/** OpenGL Implementation for the MinVR VRGraphicsToolkit abstraction.
 */
class VROpenGLGraphicsToolkit : public VRGraphicsToolkit {
public:
	PLUGIN_API VROpenGLGraphicsToolkit();
	PLUGIN_API virtual ~VROpenGLGraphicsToolkit();

	PLUGIN_API std::string getName() { return "VROpenGLGraphicsToolkit"; }
  
	PLUGIN_API void clearScreen();
	PLUGIN_API void setDrawBuffer(VRDRAWBUFFER buffer);
	PLUGIN_API void setViewport(VRRect rect);
	PLUGIN_API void flushGraphics();
	PLUGIN_API void finishGraphics();

	PLUGIN_API static VRGraphicsToolkit* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);
};


} // end namespace

#endif
