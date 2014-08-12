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

#include <musikCore/Config.h>
#include <musikCore/Task.h>

#include <OpenThreads/Mutex>
#include <queue>

///////////////////////////////////////////////////

#if !defined(__MUSIKCORE_TASKQUEUE_H__)
#define __MUSIKCORE_TASKQUEUE_H__

///////////////////////////////////////////////////

namespace musik { namespace Core {

///////////////////////////////////////////////////

class DLLEXPORT TaskQueue
{
    friend class TaskQueueWorker;

public:

    TaskQueue();
    ~TaskQueue();

    void Flush();

    void Push(Task* ptrThread);
    size_t size();
    Task* front();
    void pop();

private:

    queue<Task*> m_Queue;

    class TaskQueueWorker : public Task
    {
        friend class TaskQueue;

    public:

        virtual ~TaskQueueWorker();

        void SetQueue(TaskQueue* queue);
        void run();
        void Flush(){ m_FlushQueue = true; }

    private:

        bool m_FlushQueue;
        TaskQueue* m_TaskQueue;

    } m_Worker;

    OpenThreads::Mutex m_Mutex;

};

///////////////////////////////////////////////////

} }     // namespace musik::Core

///////////////////////////////////////////////////

#endif  // #define __MUSIKCORE_TASKQUEUE_H__

///////////////////////////////////////////////////

