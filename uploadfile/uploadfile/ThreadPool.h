#pragma once
#include <queue>
#include <string>
#include <thread>         // std::thread
#include <mutex>          // std::mutex

typedef void (*p_task_process_function)(void* param);

//线程池
class CThreadPool
{
private: //单例
	CThreadPool();
public:
	~CThreadPool();
	//静态对象单例
	static CThreadPool* Instance();

public:
	/*
   *功能：启动线程池
   *输入：线程池任务个数,实际处理任务的功能函数
   *输出：int 成功创建的线程数
   */
	int StartPool(int iThreadCount, p_task_process_function pfunc);

	/*
   *功能：停止线程池
   *输入：
   *输出：
   */
	void StopPool();

	/*
   *功能：向线程池添加一个任务
   *输入：任务内容指针
   *输出：
   */
	void AddTask(void* taskdata);

	/*
   *功能：从线程池任务列表中取出一个任务进行处理
   *输入：
   *输出： 要处理的任务的数据内容
   */
	void* PopTask();

	/*
   *功能：线程处理函数
   *输入：线程池管理类指针
   *输出： 
   */
	static void thread_function(void* param);

	/*
   *功能：实际线程处理任务函数,从队列取出任务进行处理
   *输入：
   *输出：
   */
	void run();
private:
	bool m_bCircle;									//线程循环控制变量
	int m_iThreadCount;								//线程总数
	std::queue<void*> m_queTaskData;				//线程池任务队列
	p_task_process_function m_ptaskprocessfunction; //线程池任务实际处理函数
	std::mutex  m_mutex;							//线程池任务队列锁
};

