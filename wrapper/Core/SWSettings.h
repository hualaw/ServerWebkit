#ifndef _SERVER_WEBKIT_WRAPPER_SETTINGS_H__
#define _SERVER_WEBKIT_WRAPPER_SETTINGS_H__

#include <QtCore>
#include <Assertions.h>

const static char* SW_PATH_ENV = "SW_MAIN_PATH";
const static char* CONFIG_FILE = "/config/webkit.ini";
const static char* SERVER_GROUP = "ServerConfig";
const static char* RUNLOG_PREFIX = "ServerWebkit_Run";
const static char* ERRLOG_PREFIX = "ServerWebkit_Error";
const static char* DBGLOG_PREFIX = "ServerWebkit_Debug";

// The default listening port
const static uint DEFAULT_SERVER_PORT = 3000;

// Maximum connection number by default
const static uint DEFAULT_MAX_CONNECTIONS = 10000;

// Max number of webkit instances that can be created
//const static int DEFAULT_MAX_WEBKIT_INSTANCES = 100;
const static int DEFAULT_MAX_WEBKIT_INSTANCES = 1;

// All data must be ready for processing within 10 seconds by default
const static qint64 DEFAULT_DATA_WAITING_TIME_LIMIT = 10000;

// Requests must be handled within 20 seconds by default
const static qint64 DEFAULT_REQUEST_HANDLING_TIME_LIMIT = 20000;

//Request must be finish processing within 600 seconds by default
const static qint64 DEFAULT_PROCESS_TIME_LIMIT = 600000;

// Trigger operations every 10 seconds
const static int MONITOR_INTERVAL = 10000;

const static qint64 DEFAULT_MIN_THREAD_COUNT = 1;

const static qint64 DEFAULT_MAX_THREAD_COUNT = 10;

const static qint64 DEFAULT_WINDOW_COUNT = 10;

const static qint64 DEFAULT_SEND_BUFFER_SIZE = 1048576;

class SWSettings: public QObject
{
	Q_OBJECT

public:
	SWSettings();
	~SWSettings();

	uint getServerPort();
	uint getMaxConnections();
	int getMaxWebkitInstances();
	qint64 getDataWaitingTimeLimit();
	qint64 getRequestHandlingTimeLimit();
	qint64 getMaxProcessTimeLimit();
	qint64 getMinThreadCount();
	qint64 getMaxThreadCount();
	int getMonitorInterval();
    QString getRunLogPrefix();
    QString getErrLogPrefix();
    QString getDbgLogPrefix();
    int getWindowCount();
    int getSendBufferSize();

private:
	QSettings *settings;

	uint serverPort;
	uint maxConnections;
	int maxWebkitInstances;
	qint64 dataWaitingTimeLimit;
	qint64 requestHandlingTimeLimit;
	qint64 maxProcessTimeLimit;
    QByteArray swPath;
};

#endif // _SERVER_WEBKIT_WRAPPER_SETTINGS_H__
