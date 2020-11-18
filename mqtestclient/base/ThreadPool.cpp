/*
 * ThreadPool.cpp
 *
 *  Created on: 2014年7月21日
 *      Author: ziteng
 */

#include <stdlib.h>
#include "util.h"
#include "ThreadPool.h"

///////////
CWorkerThread::CWorkerThread() {
    m_deal_task_cnt = 0;
    m_cur_task_cnt = 0;
    m_run_flag = true;
}

CWorkerThread::~CWorkerThread() {
    m_run_flag = false;
    //m_thread_notify.Signal();
    //while (m_task_list.empty()){
    //    CTask *pTask = m_task_list.front();
    //    m_task_list.pop_front();
    //    delete pTask;
    //}
}

void *CWorkerThread::StartRoutine(void *arg) {
    CWorkerThread *pThread = (CWorkerThread *) arg;

    pThread->Execute();

    return NULL;
}

void CWorkerThread::Start() {
    (void) pthread_create(&m_thread_id, NULL, StartRoutine, this);
}

void CWorkerThread::Execute() {
    while (m_run_flag) {
        m_thread_notify.Lock();

        // put wait in while cause there can be spurious wake up (due to signal/ENITR)
        //while (m_task_list.empty() && m_run_flag) {
        while (m_task_list.empty()) {
            m_thread_notify.Wait();
        }
        //if(!m_run_flag){
        //    m_thread_notify.Unlock();
        //    return;
        //}

        CTask *pTask = m_task_list.front();
        m_task_list.pop_front();
        m_cur_task_cnt = m_task_list.size();
        m_thread_notify.Unlock();

        pTask->run();

        delete pTask;

        m_deal_task_cnt++;
    }
}

void CWorkerThread::PushTask(CTask *pTask) {
    m_thread_notify.Lock();
    m_task_list.push_back(pTask);
    m_thread_notify.Signal();
    m_thread_notify.Unlock();
}

//////////////
CThreadPool::CThreadPool() {
    m_worker_size = 0;
    m_worker_list = NULL;
}

CThreadPool::~CThreadPool() {

}

int CThreadPool::Init(uint32_t worker_size) {
    m_worker_size = worker_size;
    m_worker_list = new CWorkerThread[m_worker_size];
    if (!m_worker_list) {
        return 1;
    }

    for (uint32_t i = 0; i < m_worker_size; i++) {
        m_worker_list[i].SetThreadIdx(i);
        m_worker_list[i].Start();
    }

    return 0;
}

void CThreadPool::Destory() {
    if (m_worker_list)
        delete[] m_worker_list;
}

void CThreadPool::AddTask(CTask *pTask) {
    /*
     * select a random thread to push task
     * we can also select a thread that has less task to do
     * but that will scan the whole thread list and use thread lock to get each task size
     */
    uint32_t thread_idx = random() % m_worker_size;
    m_worker_list[thread_idx].PushTask(pTask);
}

void CThreadPool::PrintStatus() {
    for (uint32_t i = 0; i < m_worker_size; i++) {
        CWorkerThread t = m_worker_list[i];
        DEBUG("threadId:%d,execute task count=%d,cur_task_cnt=%d", t.GetThreadIdx(), t.GetDealTaskCount(),
              t.GetCurTaskCount());
    }
}

