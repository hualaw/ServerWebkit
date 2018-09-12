#ifndef _SERVER_WEBKIT_WRAPPER_QTHREAD_MANAGER_H__
#define _SERVER_WEBKIT_WRAPPER_QTHREAD_MANAGER_H__

#include <QThread>
#include <QTimer>
#include <QWaitCondition>
#include <QNetworkProxy>
#include <QReadWriteLock>
#include <QSemaphore>
#include <QQueue>
#include "SWSettings.h"
#include "JobProcessor.h"

enum ThreadState{
	RUNNING,
	LOCKED,
};

struct ThreadInfo
{
	JobProcessor *jp; //jp thread
	QWaitCondition *cond; // thread's wait for cond
};

class QThreadManager : public QObject
{
	Q_OBJECT
public:
	QThreadManager(SWSettings *settings, QNetworkProxy proxy, QQueue<int> *qu, QReadWriteLock *lock, QSemaphore *p_sem, QObject *parent=0);
	~QThreadManager();
    JobProcessor *getWorker();

private:
	QList<JobProcessor *> threadList;
	int minThreadCount;
	int maxThreadCount;
	int actualThreadCount;
	//QTimer *pMonitor;
	QReadWriteLock *lock;
	QNetworkProxy proxy;
	SWSettings *settings;
	QQueue<int> *qu;
    QSemaphore *p_sem;
};

#endif
