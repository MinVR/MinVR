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

int main(int argc, char **argv) {
    
    // argc, argv need to be passed to the app so that MinVR can parse command
    // line args to see which config files to load.
	UIExample app(argc, argv);

    // Does not return until the program shuts down.
    app.run();

    return 0;
}
