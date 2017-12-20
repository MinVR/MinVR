/** 
This file is part of the MinVR Open Source Project, which is developed and 
maintained collaboratively by the University of Minnesota and Brown University.

Copyright (c) 2016 Regents of the University of Minnesota and Brown University.
This software is distributed under the BSD-3 Clause license, which can be found
at: MinVR/LICENSE.txt.

Original Author(s) of this File: 
  Dan Keefe, 2017, University of Minnesota
  
Author(s) of Significant Updates/Modifications to the File:
  ... 
*/


#ifndef VRLOG_H
#define VRLOG_H

#include <string>
#include <iostream>

namespace MinVR {

// TODO: Eventually, we'll want to update this to (at least optionally) write to 
// a file rather than stdout.  We might also need to have multiple log files for
// multiple processes and threads.  So, this likely needs to get a bit more
// complex.  For now, we need to start getting some log messages into the code 
// ASAP, and it's fine if these print to stdout as we are developing.

#define VRLOG_H1(heading1) { \
    std::string str = std::string("\n\n==== ") + std::string(heading1) + std::string(" ====\n");   \
    std::cout.write(str.c_str(), str.size()); \
}

#define VRLOG_H2(heading2) { \
    std::string str = std::string("\n* ") + std::string(heading2) + std::string("\n");   \
    std::cout.write(str.c_str(), str.size()); \
}

#define VRLOG_STATUS(message) { \
    std::string str = std::string("- ") + std::string(message) + std::string("\n");   \
    std::cout.write(str.c_str(), str.size()); \
}


} // end namespace

#endif
