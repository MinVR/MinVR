/* ================================================================================

This file is part of the MinVR Open Source Project, which is developed and
maintained by the University of Minnesota's Interactive Visualization Lab.

File: MinVR/MVRCore/include/MVRCore/Thread.h

Original Author(s) of this File:
	Dan Orban, 2015, University of Minnesota

Author(s) of Significant Updates/Modifications to the File:
	...

-----------------------------------------------------------------------------------
Copyright (c) 2015 Regents of the University of Minnesota
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* The name of the University of Minnesota, nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================ */

#ifndef THREAD_H_
#define THREAD_H_

#include "plugin/VRPlugin.h"

#ifdef USE_BOOST
#include "Boost/BoostThread.h"
#else
#include <thread>
#include <mutex>
#include <condition_variable>
#endif

// If we are using c++11, typedef the c++11 objects to common ones
#ifndef USE_BOOST
namespace MinVR
{

typedef std::mutex Mutex;
typedef std::condition_variable ConditionVariable;
typedef std::thread Thread;
typedef std::unique_lock<Mutex> UniqueMutexLock;

/**
 * Barrier acts as a synchronization point for threads
 */
class Barrier
{
public:
	PLUGIN_API Barrier(unsigned int numThreads) : _numThreads(numThreads), m_count(numThreads), m_generation(0) {}
	PLUGIN_API ~Barrier() {}

	PLUGIN_API bool wait()
    {
      UniqueMutexLock lock(m_mutex);
      unsigned int gen = m_generation;

      if (--m_count == 0)
      {
        m_generation++;
        m_count = _numThreads;
        m_cond.notify_all();
        return true;
      }

      while (gen == m_generation)
        m_cond.wait(lock);
      return false;
    }


private:
    unsigned int _numThreads;
    Mutex m_mutex;
    ConditionVariable m_cond;
    unsigned int m_count;
    unsigned int m_generation;
};

}
#endif

#endif /* THREAD_H_ */
