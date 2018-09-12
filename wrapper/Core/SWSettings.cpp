#include <QApplication>
#include "SWSettings.h"

SWSettings::SWSettings()
:serverPort(DEFAULT_SERVER_PORT), maxConnections(DEFAULT_MAX_CONNECTIONS),
maxWebkitInstances(DEFAULT_MAX_WEBKIT_INSTANCES),
dataWaitingTimeLimit(DEFAULT_DATA_WAITING_TIME_LIMIT),
requestHandlingTimeLimit(DEFAULT_REQUEST_HANDLING_TIME_LIMIT)
{
	// Make up config file path
	swPath = qgetenv(SW_PATH_ENV);
	if (swPath.isEmpty())
	{
		// If environment variable is not set, use local path
		swPath = "..";
	}

	QString configFilePath(swPath);
	configFilePath.append(CONFIG_FILE);

    if( !(QFile(configFilePath).exists()) )
    {
        qDebug() << "ServerWebkit config file " << configFilePath << " does not exist!";
        QApplication::quit();
    }

	// Load config file
	settings = new QSettings(configFilePath, QSettings::IniFormat, this);
	settings->beginGroup(SERVER_GROUP);
}

SWSettings::~SWSettings()
{
    qDebug() << "SWSettings's destruction function is called";
	if (settings !=	NULL)
	{
		settings->endGroup();
		delete settings;
        settings = NULL;
	}
}

uint SWSettings::getServerPort()
{
	serverPort = settings->value("Port", DEFAULT_SERVER_PORT).toUInt();
	return serverPort;
}

uint SWSettings::getMaxConnections()
{
	maxConnections = settings->value("Connection", DEFAULT_MAX_CONNECTIONS).toUInt();
	return maxConnections;
}

int SWSettings::getMaxWebkitInstances()
{
	maxWebkitInstances = settings->value("WebkitInstances", DEFAULT_MAX_WEBKIT_INSTANCES).toInt();
	return maxWebkitInstances;
}

qint64 SWSettings::getDataWaitingTimeLimit()
{
	dataWaitingTimeLimit = settings->value("DataWaitingTimelimit", DEFAULT_DATA_WAITING_TIME_LIMIT).toULongLong();
	return dataWaitingTimeLimit;
}

qint64 SWSettings::getRequestHandlingTimeLimit()
{
	requestHandlingTimeLimit = settings->value("RequestHandlingTimeLimit", DEFAULT_REQUEST_HANDLING_TIME_LIMIT).toULongLong();
	return requestHandlingTimeLimit;
}

qint64 SWSettings::getMaxProcessTimeLimit()
{
	maxProcessTimeLimit = settings->value("MaxProcessTimeLimit", DEFAULT_PROCESS_TIME_LIMIT).toULongLong();
	return maxProcessTimeLimit;
}

int SWSettings::getMonitorInterval()
{
	return settings->value("MonitorInterval", MONITOR_INTERVAL).toInt();
}

qint64 SWSettings::getMinThreadCount()
{
	return settings->value("MinThreadCount", DEFAULT_MIN_THREAD_COUNT).toULongLong();
}

qint64 SWSettings::getMaxThreadCount()
{
	return settings->value("MaxThreadCount", DEFAULT_MAX_THREAD_COUNT).toULongLong();
}

QString SWSettings::getRunLogPrefix()
{
	return settings->value("RunLogPrefix", RUNLOG_PREFIX).toString();
}

QString SWSettings::getErrLogPrefix()
{
	return settings->value("ErrLogPrefix", ERRLOG_PREFIX).toString();
}

QString SWSettings::getDbgLogPrefix()
{
	return settings->value("DbgLogPrefix", DBGLOG_PREFIX).toString();
}

int SWSettings::getWindowCount()
{
	return settings->value("WindowCount", DEFAULT_WINDOW_COUNT).toInt();
}

int SWSettings::getSendBufferSize()
{
	return settings->value("SendBufferSize", DEFAULT_SEND_BUFFER_SIZE).toInt();
}
