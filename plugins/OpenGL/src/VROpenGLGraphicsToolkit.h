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

	PLUGIN_API std::string getName() const { return "VROpenGLGraphicsToolkit"; }
  
	PLUGIN_API void setDrawBuffer(VRDRAWBUFFER buffer);
	PLUGIN_API void setSubWindow(VRRect rect);
	PLUGIN_API void flushGraphics();
	PLUGIN_API void finishGraphics();

	PLUGIN_API static VRGraphicsToolkit* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);
};


} // end namespace

#endif
