///////////////////////////////////////////////////
//
// License Agreement:
//
// The following are Copyright � 2002-2006, Casey Langen
//
// Sources and Binaries of: musikCore musikCube musikBox
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright 
//      notice, this list of conditions and the following disclaimer in the 
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may 
//      be used to endorse or promote products derived from this software 
//      without specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE. 
//
///////////////////////////////////////////////////

#if !defined(__MUSIKCORE_FUNCTOR_H__)
#define __MUSIKCORE_FUNCTOR_H__

///////////////////////////////////////////////////

#include <musikCore/Config.h>
#include <musikCore/Task.h>

///////////////////////////////////////////////////

namespace musik { namespace Core {

///////////////////////////////////////////////////

/**
An object that acts as a virtual interface between musikCore
and an application or plugin.

Task objects, including Player are constructed with
a Functor as an argument. A practical example of the usage:

When a new song starts, Player calls its internal functor's 
OnNewSong() function. This function in return posts an event to 
an event to it's parent frame.

All the functions are optional to override and only exist to provide
an easy means for any type of interface to manage Tasks.

@author Casey Langen
*/
class DLLEXPORT Functor
{
public:

    /**
     * Default constructor.
     */
    Functor();
    virtual ~Functor();

    /**
     * Called by Player when a new song starts
     */
    virtual void OnNewSong();
    /**
     * Called by Player when playback is paused
     */
    virtual void OnPause();
    /**
     * Called by Player when playback is resumed
     */
    virtual void OnResume();
    /**
     * Called by Player when playback is stopped
     */
    virtual void OnStop();
    /**
     * Task objects, with the exception of CmusikPlayerWorker
     * should call this at the beginning of their Run() 
     * function
     */
    virtual void OnTaskStart();
    /**
     * Task objects, with the exception of CmusikPlayerWorker,
     * should call this function and pass "this" as the parameter. 
     * This allows a UI to be notified when a task has completed so
     * it can delete the object, if necessary, decrement a counter, etc.
     *
     * \param task_addr address of the task that called the function
     */
    virtual void OnTaskEnd(Task* task_addr = NULL);
    /**
     * Cube uses this as a completion % gauge
     *
     * \param progress percent of progress
     * \param task_addr address of task that posted called the function
     */
    virtual void OnTaskProgress(size_t progress, Task* task_addr = NULL);
    /**
     * A new song attempted to start playback but failed.
     */
    virtual void OnPlaybackFailed();
    /**
     * The player loaded a new equalizer
     */
    virtual void OnNewEqualizer();
    /**
     * The thread in question requests verification. This is considered
     * musikCube specific, but can be applicable elsewhere
     *
     * \param pl_addr 
     * \return 
     */
    virtual bool VerifyPlaylist(void* pl_addr = NULL);
    /**
     * A musik::Core::Alarm will have the player call this event when it
     * it is triggered. musik::Core::Player::TriggerAlarm() is public,
     * so other controls can send this event manually, but this is
     * not recommended.
     */
    virtual void OnAlarmTrigger();

    /**
      * Player has just seeked.
      */
    virtual void OnSeek();

};

///////////////////////////////////////////////////

} }     // namespace musik::Core

///////////////////////////////////////////////////

#endif  // #if !defined(__MUSIKCORE_FUNCTOR_H__)

///////////////////////////////////////////////////
