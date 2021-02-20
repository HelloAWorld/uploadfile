#pragma once
#include <queue>
#include <string>
#include <thread>         // std::thread
#include <mutex>          // std::mutex

typedef void (*p_task_process_function)(void* param);

//�̳߳�
class CThreadPool
{
private: //����
	CThreadPool();
public:
	~CThreadPool();
	//��̬������
	static CThreadPool* Instance();

public:
	/*
   *���ܣ������̳߳�
   *���룺�̳߳��������,ʵ�ʴ�������Ĺ��ܺ���
   *�����int �ɹ��������߳���
   */
	int StartPool(int iThreadCount, p_task_process_function pfunc);

	/*
   *���ܣ�ֹͣ�̳߳�
   *���룺
   *�����
   */
	void StopPool();

	/*
   *���ܣ����̳߳����һ������
   *���룺��������ָ��
   *�����
   */
	void AddTask(void* taskdata);

	/*
   *���ܣ����̳߳������б���ȡ��һ��������д���
   *���룺
   *����� Ҫ������������������
   */
	void* PopTask();

	/*
   *���ܣ��̴߳�����
   *���룺�̳߳ع�����ָ��
   *����� 
   */
	static void thread_function(void* param);

	/*
   *���ܣ�ʵ���̴߳���������,�Ӷ���ȡ��������д���
   *���룺
   *�����
   */
	void run();
private:
	bool m_bCircle;									//�߳�ѭ�����Ʊ���
	int m_iThreadCount;								//�߳�����
	std::queue<void*> m_queTaskData;				//�̳߳��������
	p_task_process_function m_ptaskprocessfunction; //�̳߳�����ʵ�ʴ�����
	std::mutex  m_mutex;							//�̳߳����������
};

