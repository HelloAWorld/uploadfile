#include "ThreadPool.h"
#include <iostream>
#include <unistd.h>
#include "mongoose.h"

#ifdef _WIN32
#else
#define closesocket(x) close(x)
#include <pthread.h>
#endif

CThreadPool::CThreadPool()
{
    m_bCircle = false;
}

CThreadPool::~CThreadPool()
{
}

CThreadPool* CThreadPool::Instance()
{
	static CThreadPool gThreadPool;
	return &gThreadPool;
}

int CThreadPool::StartPool(int iThreadCount, p_task_process_function pfunc)
{
    m_bCircle = true;
    m_ptaskprocessfunction = pfunc;

    for (int index = 0; index < iThreadCount; index++) 
    {
        std::thread th(thread_function, this);
        th.detach();
    }
    return 0;
}

void CThreadPool::StopPool()
{
    m_bCircle = false;
}

void CThreadPool::AddTask(void* taskdata)
{
    m_mutex.lock();
    m_queTaskData.push(taskdata);
    m_mutex.unlock();
}

void* CThreadPool::PopTask()
{
    
    void* taskdata = nullptr;
    m_mutex.lock();
    if (m_queTaskData.size() > 0)
    {
        taskdata = m_queTaskData.front();
        m_queTaskData.pop();
    }
    m_mutex.unlock();

    return taskdata;
}

void CThreadPool::thread_function(void* param)
{
    CThreadPool* pthreadpool = (CThreadPool*)param;

    LOG(LL_INFO, ("CThreadPool::thread_function thread id:%ld start", pthread_self()));;
    pthreadpool->run();
}

void CThreadPool::run()
{
    while (m_bCircle)
    {
        void* taskdata = PopTask();
        if (taskdata != nullptr)
        {
            m_ptaskprocessfunction(taskdata);
        }
        usleep(10);
    }
}
