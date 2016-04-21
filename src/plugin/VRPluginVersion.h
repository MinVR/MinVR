#ifndef PLUGINVERSION_H
#define PLUGINVERSION_H

#define PLUGIN_FRAMEWORK_VERSION 0

extern "C"
{
	PLUGIN_API int getPluginFrameworkVersion() {
		return PLUGIN_FRAMEWORK_VERSION;
	}
}

#endif
