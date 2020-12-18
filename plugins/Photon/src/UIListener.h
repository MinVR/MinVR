#pragma once

#include <memory>
#include "Common-cpp/inc/Common.h"

class UIListener
{
public:
	virtual ~UIListener(void){};
	virtual void writeString(const ExitGames::Common::JString& str) = 0;
	virtual bool anyKeyPressed(void) const = 0;
	virtual void onLibClosed(void) = 0;
};