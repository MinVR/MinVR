//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

/*
 * \author Andy Foresberg (asf) and Daniel Keefe (dfk)
 *
 * \file  SynchedSystem.h
 *
 */



#ifndef SYNCHEDSYSTEM_H
#define SYNCHEDSYSTEM_H

#include "CommonInc.H"

namespace MinVR {


/** This is a static class that reproduces some of the functionality
    of G3D::System for use specifically in clustered rendering
    environments.  For example, when doing animation based on the
    system's clock you want the time to be the same across all the
    rendering nodes, so you can't use System::getLocalTime() because
    it will be slightly off between the various nodes.  Instead, use
    SynchedSystem::getLocalTime() and the value for time will be the
    same across all walls, although, it will only be as accurate as
    the rate at which the time is updated from the vrg3d event server,
    which will be once per frame.
*/
  PLUGIN_API class SynchedSystem
{
public:

  /// Programs should access the time through these methods
  PLUGIN_API static double getLocalTime();
  PLUGIN_API static double getAppStartTime(); 
  PLUGIN_API static double getAppRunningTime(); 

  PLUGIN_API enum TimeUpdateMethod {
    USE_LOCAL_SYSTEM_TIME,  // used in non-cluster situations (default)
    PROGRAM_UPDATES_TIME    // used in clustered situations
  };

  /// Called by VRApp when running as a cluster client to initialize
  /// the system.
  PLUGIN_API static void setProgramUpdatesTime(double applicationStartTime);

  /// Called by VRApp when running as a cluster client once per frame
  /// when it receives a new timing event from the synchronization
  /// server.
  PLUGIN_API static void updateLocalTime(double t);

  PLUGIN_API static TimeUpdateMethod getTimeUpdateMethod(); 

protected:
  PLUGIN_API static TimeUpdateMethod _timeUpdateMethod;
  PLUGIN_API static double           _appStartTime;
  PLUGIN_API static double           _localTime;
};

} // end namespace

#endif
