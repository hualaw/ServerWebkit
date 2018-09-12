#include "SWLogger.h"

SWLogger::SWLogger()
    :runLogWp(NULL),errLogWp(NULL),dbgLogWp(NULL)
{
    qDebug() << "enter SWLogger construction function";
	QByteArray swPath = qgetenv(SW_PATH_ENV);
	if (swPath.isEmpty())
	{
		// If environment variable is not set, use upper path
		swPath = "..";
	}

    QString logDir(swPath + "/logs/");

    //if dir is not exist, create it
    QDir qtLogDir(logDir);
    if( !qtLogDir.exists() )
    {
        if(QDir(swPath).mkdir("logs"))
        {
            qDebug() << "mkdir " << logDir << " success!";
        }
        else
        {
            qDebug() << "mkdir " << logDir << " failed!";
        }
    }

    QString runlog_file = logDir + "run.log";
    QString errlog_file = logDir + "error.log";
    QString dbglog_file = logDir + "debug.log";

    runLogWp = new Log4cplusWrapper(runlog_file.toUtf8().data(), settings.getRunLogPrefix().toUtf8().data(), DAILY, true, 0);
    errLogWp = new Log4cplusWrapper(errlog_file.toUtf8().data(), settings.getErrLogPrefix().toUtf8().data(), DAILY, true, 0);
    dbgLogWp = new Log4cplusWrapper(dbglog_file.toUtf8().data(), settings.getDbgLogPrefix().toUtf8().data(), DAILY, true, 0);
    //dbgLogWp = new Log4cplusWrapper("/home/liuhua/ServerWebkit/logs/debug.log", "ServerWebkitDebug");
}

SWLogger::~SWLogger()
{
    qDebug() << "enter into SWLogger start destruction function";
    if(runLogWp != NULL)
    {
        delete runLogWp;
        runLogWp = NULL;
        qDebug() << "enter into SWLogger runLogWp destruction function";
    }
    if(dbgLogWp != NULL)
    {
        delete dbgLogWp;
        dbgLogWp = NULL;
        qDebug() << "enter into SWLogger dbgLogWp destruction function";
    }
    if(errLogWp != NULL)
    {
        delete errLogWp;
        errLogWp = NULL;
        qDebug() << "enter into SWLogger errLogWp destruction function";
    }
    qDebug() << "enter into SWLogger end destruction function";
}

Log4cplusWrapper *SWLogger::get_handle(std::string type)
{
    if(type.compare("ERROR") == 0)
    {
        return errLogWp;
    }
    else if(type.compare("DEBUG") == 0)
    {
        return dbgLogWp;
    }
    else
    {
        return runLogWp;
    }
}

//void SWLogger::log(const char *type, const char *message)
void SWLogger::log(const char *type, QString message)
{
    if(strcmp(type,"ERROR") == 0 && errLogWp)
    {
        //LOG4CPLUS_ERROR(errLogWp->GetLogger(), message);
        LOG4CPLUS_ERROR(errLogWp->GetLogger(), message.toUtf8().data());
    }
    else if(strcmp(type, "DEBUG") == 0  && dbgLogWp)
    {
        //LOG4CPLUS_DEBUG(dbgLogWp->GetLogger(), message);
        LOG4CPLUS_DEBUG(dbgLogWp->GetLogger(), message.toUtf8().data());
    }
    else if(runLogWp)
    {
        //LOG4CPLUS_INFO(runLogWp->GetLogger(), message);
        LOG4CPLUS_INFO(runLogWp->GetLogger(), message.toUtf8().data());
    }
}
