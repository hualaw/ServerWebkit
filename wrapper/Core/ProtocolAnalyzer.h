#ifndef _SERVER_WEBKIT_WRAPPER_PROTOCOLANALYZER_H_
#define _SERVER_WEBKIT_WRAPPER_PROTOCOLANALYZER_H_

#include "SWGlobal.h"

class JobProcessor;

const static int BEGINNING_SIZE = 3;
const static int ENDING_SIZE = 3;

const static QString CTRL_OS_KEY = "x-zmctrl-os";
const static QString CTRL_UID_KEY = "x-zmctrl-uid";
const static QString CTRL_TID_KEY = "x-zmctrl-tid";
const static QString CTRL_PID_KEY = "x-zmctrl-pid";
const static QString CTRL_L_KEY = "x-zmctrl-l";
const static QString CTRL_SCREEN_WIDTH_KEY = "x-zmctrl-scrwidth";
const static QString CTRL_SCREEN_HEIGHT_KEY = "x-zmctrl-scrheight";
const static QString CTRL_LS_KEY = "x-zmctrl-ls";
const static QString CTRL_PQ_KEY = "x-zmctrl-pq";
const static QString CTRL_SEQ_KEY = "x-zmctrl-seq";
const static QString CTRL_DELETE_COOKIE_KEY = "x-zmctrl-delcookie";
const static QString CTRL_F_KEY = "x-zmctrl-f";
const static QString CTRL_DEVISION_STR = "\r\n";

//add by Tot 12.06.10 for debug webkit wrapper ------------------
const static QString CTRL_PV_KEY = "x-zmctrl-pv";
// --------------


enum AnalyzerState
{
	NOT_STARTED = 0,
	WAIT_FOR_BEGINNING,
	WAIT_FOR_FULL_DATA,
	PROCESSING_CONTENT,
	HANDLING_REQUEST,
	COMPLETED
};

class ProtocolAnalyzer: public QObject
{
	Q_OBJECT

public:
	ProtocolAnalyzer(SWClientSocket *, WebkitWrapper *, JobProcessor *jp, int pid, QObject * parent = 0);
	~ProtocolAnalyzer();

public slots:
	int doAnalysis();

private:
	void errorExit( QString );
	int processContent( QByteArray );
	QhEventStruct* makeRequest( QByteArray );
	int processHeader( QString, QhEventStruct* );
	void readPair(QString &src, QString &key, QString &value, int &start, int tb);

	void handleRequest(QhEventStruct*, QhEventResponse*);

private:
	qint64 birthTime;
	AnalyzerState state;
	SWClientSocket *pClientSocket;
	WebkitWrapper *webkit;
    JobProcessor *jp;
    int pid;
    QhEventStruct *pRequest;
    QhEventResponse *pResponse;
};


#endif // _SERVER_WEBKIT_WRAPPER_PROTOCOL_ANALYZER_H_
