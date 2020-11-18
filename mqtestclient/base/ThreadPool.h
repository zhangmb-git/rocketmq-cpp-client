/*
 * ThreadPool.h
 *
 *  Created on: 2014年7月21日
 *      Author: ziteng
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_
#include "ostype.h"
#include "Thread.h"
#include "Task.h"
#include <pthread.h>
#include <list>
using namespace std;

class CWorkerThread {
public:
	CWorkerThread();
	~CWorkerThread();

	static void* StartRoutine(void* arg);

	void Start();
	void Execute();
	void PushTask(CTask* pTask);

	void SetThreadIdx(uint32_t idx) { m_thread_idx = idx; }
    uint32_t GetThreadIdx(){ return m_thread_idx;}
    uint32_t GetCurTaskCount(){ return m_cur_task_cnt;}
    uint32_t GetDealTaskCount(){ return m_deal_task_cnt;}

private:
	uint32_t		m_thread_idx;
	uint32_t		m_deal_task_cnt;
	uint32_t        m_cur_task_cnt;
    bool            m_run_flag;

	pthread_t		m_thread_id;
	CThreadNotify	m_thread_notify;
	list<CTask*>	m_task_list;
};

class CThreadPool {
public:
	CThreadPool();
	virtual ~CThreadPool();

	int Init(uint32_t worker_size);
	void AddTask(CTask* pTask);
	void Destory();
	void PrintStatus();
private:
	uint32_t 		m_worker_size;
	CWorkerThread* 	m_worker_list;
};



#endif /* THREADPOOL_H_ */
