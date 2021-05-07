#include <errno.h>
#include <string.h>


#include "SLSThread.hpp"
#include "SLSLog.hpp"

/**
 * CSLSThread class implementation
 */

CSLSThread::CSLSThread()
{
	m_exit     = 0;
	m_th_id     = 0;
}
CSLSThread::~CSLSThread()
{
	stop();
}

int CSLSThread::start()
{
	int ret = 0;
    int err;
    pthread_t th_id;

    err = pthread_create(&th_id, NULL, thread_func, (void *)this);
    if (err != 0) {
    	sls_log(SLS_LOG_ERROR, "[%p]CSLSThread::start, can't create thread, error: %s\n", this, strerror(err));
        return -1;
    }
    m_th_id = th_id;
    sls_log(SLS_LOG_INFO, "[%p]CSLSThread::start, pthread_create ok, m_th_id=%lld.", this, m_th_id);

	return ret;

}
int CSLSThread::stop()
{
	int ret = 0;
    if (0 == m_th_id) {
        return ret;
    }
    sls_log(SLS_LOG_INFO, "[%p]CSLSThread::stop, m_th_id=%lld.", this, m_th_id);

    m_exit = 1;
	pthread_join(m_th_id, NULL);
	m_th_id = 0;
    clear();

	return ret;

}

void CSLSThread::clear()
{

}

bool  CSLSThread::is_exit()
{
	return m_exit == 1;
}

void * CSLSThread::thread_func(void * arg)
{
	CSLSThread *pThis = (CSLSThread *)arg;
	if (!pThis)
	{
    	sls_log(SLS_LOG_ERROR, "CSLSThread::thread_func, thread arg is null.\n");
	}

	pThis->work();
	return NULL;
}


int CSLSThread::work()
{
	int ret = 0;

	return ret;

}
