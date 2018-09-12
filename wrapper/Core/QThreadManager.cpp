#include <QApplication>
#include "QThreadManager.h"
#include "Xlib.h"
#include "SWLogger.h"

extern SWLogger gLog;
extern Log4cplusWrapper *errLogWp;
extern Log4cplusWrapper *dbgLogWp;
extern Log4cplusWrapper *runLogWp;


QThreadManager::QThreadManager(SWSettings *settings, QNetworkProxy proxy, QQueue<int> *qu, QReadWriteLock *lock, QSemaphore *p_sem, QObject *parent)
: QObject(parent), lock(lock), proxy(proxy), settings(settings), qu(qu), p_sem(p_sem)
{
	minThreadCount = settings->getMinThreadCount();
	maxThreadCount = settings->getMaxThreadCount();

	if(minThreadCount <= 0 && maxThreadCount < minThreadCount)
	{
        QString errlog = QString("minThreadCount %1 is less than 0 or maxThreadCount %2 < minThreadCount %3").arg(minThreadCount).arg(maxThreadCount).arg(minThreadCount);
        qDebug() << errlog;
        gLog.log("ERROR", errlog);
        QApplication::quit();
	}

    //Init Threads to make Xlib to support multi-threads
    int init_result = (int)(XInitThreads());
    if( init_result == 0 )
    {
        QString errlog = QString("QThreadManager Constructor call XInitThreads() failed!");
        qDebug() << errlog;
        gLog.log("ERROR", errlog);
        QApplication::quit();
    }

	//start minThreadCount JobProcessor Threads
	for(int i = 0; i< minThreadCount; i++)
	{
		JobProcessor *jp = new JobProcessor(settings, proxy, qu, lock, p_sem);
		//connect(jp, SIGNAL(locked()), this, SLOT(changeStatusToLocked()));
		threadList.append(jp);
		//stateList.append(LOCKED); // LOCKED

		//jp->start();
		//jp->run();

        LOG4CPLUS_DEBUG(dbgLogWp->GetLogger(), "thread[" << i << "] id is " << jp);
	}
	actualThreadCount = minThreadCount;

    QString runlog = QString("successfully start %1 thread").arg(minThreadCount);
    gLog.log("RUN", runlog);
}

QThreadManager::~QThreadManager()
{
    qDebug() << "QThreadManager's destruction function is called";
	for(int i=0; i<actualThreadCount; i++)
	{
		JobProcessor *jp = threadList.at(i);
		if(jp != NULL)
        {
            delete jp;
            jp = NULL;
        }
	}
}

JobProcessor *QThreadManager::getWorker()
{
    return threadList.last();
}

