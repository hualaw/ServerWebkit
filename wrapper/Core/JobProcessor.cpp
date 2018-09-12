#include "JobProcessor.h"

extern SWLogger gLog;
extern Log4cplusWrapper *errLogWp;
extern Log4cplusWrapper *dbgLogWp;
extern Log4cplusWrapper *runLogWp;
extern WebkitWrapper **ww;
extern metaData **md;
extern SWSettings settings;

JobProcessor::JobProcessor(QObject *parent)
:QObject(parent), pa(NULL), timer(NULL), winNum(0)
{
}

JobProcessor::~JobProcessor()
{
}

void JobProcessor::clearParams()
{
    //free all resources
    if(pa != NULL)
    {
        pa->deleteLater();
        pa = NULL;
        qDebug() << "ProtocolAnalyzer is free";
    }
    stopTimer();

    if(timer != NULL)
    {
        timer->deleteLater();
        timer = NULL;
        qDebug() << "timer is free";
    }

    qDebug() << "JobProcessor's destruction function called";
}

void JobProcessor::start(int pid)
{
    qDebug() << "enter JobProcessor::start()";
    qDebug() << "pid: " << pid;

    winNum = pid;
    socket = md[pid]->socket;
    webkit = ww[pid];

    webkit->setPid(pid);
    webkit->setJP(this);

    pa = new ProtocolAnalyzer(socket, webkit, this, pid, this);
    startTimer(this);

    //modify metaData Structure's state
    md[pid]->state = PROCESS;

    int result = pa->doAnalysis();
    if( result != 0 )
    {
        clearParams();
    }
    qDebug() << "leave JobProcessor::start()";
}

void JobProcessor::stopLoadPage()
{
    DEBUG_ENTER();
    QString currentSeconds =  QDateTime::currentDateTime().toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
    qDebug() << "timer out, stopLoadPage's Current seconds is:" << currentSeconds;
    DEBUG_INFO("current seconds is:%s", qPrintable(currentSeconds));

    //stop QWebPage continue to load current URL
    ww[winNum]->stopLoadPage();

    //avoid hang-up webkit's process
    if( md[winNum]->socket )
    {
        md[winNum]->socket->close();
        md[winNum]->socket = NULL;
        qDebug() << "stopLoadPage close socket";
    }
    DEBUG_LEAVE();
}

void JobProcessor::startTimer(QObject *obj)
{
    DEBUG_ENTER();
    //create timer and start it
    timer = new QTimer(obj);
    connect(timer, SIGNAL(timeout()), this, SLOT(stopLoadPage()));
    timer->setSingleShot(true);
    QString curTime = QDateTime::currentDateTime().toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
    qDebug() << "Current seconds is:" << curTime;
    //if process time more than 10s, free all data structrue and quit the process, write the timeout errlog
    timer->start(settings.getMaxProcessTimeLimit());
    qDebug() << "leave timer is started in startTimer(), wait " << settings.getMaxProcessTimeLimit() << "msec";
    DEBUG_LEAVE("current seconds is:%s", qPrintable(curTime));
}

void JobProcessor::stopTimer()
{
    if( timer && timer->isActive() )
    {
        timer->stop();
        qDebug() << "timer is stopped in clearParams()";
    }
}
