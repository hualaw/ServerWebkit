#ifndef _SERVER_WEBKIT_WRAPPER_PROTOCOL_ENCODER_H__
#define _SERVER_WEBKIT_WRAPPER_PROTOCOL_ENCODER_H__

#include "SWClientSocket.h"
#include "WebkitWrapper.h"
#include "SWSettings.h"

class JobProcessor;

class ProtocolEncoder: public QObject
{
	Q_OBJECT

public:
	ProtocolEncoder(SWSettings *pSettings, SWClientSocket *, WebkitWrapper *, JobProcessor *jp, QObject * parent = 0);
	~ProtocolEncoder();
    bool getEncodeSign() {return isEncodeFinished;}

public slots:
	void doEncode();
	void excptDoEncode(bool ok);

private:
	void response();
	char * getSendBuffer(qint64 &);

private:
	SWClientSocket *pClientSocket;
	WebkitWrapper *webkitWrapper;
	QhEventStruct *pRequest;
	QhEventResponse *pResponse;
	char *buffer;
	SWSettings *pSettings;
    JobProcessor *jp;
    bool isEncodeFinished;
};


#endif // _SERVER_WEBKIT_WRAPPER_PROTOCOL_ENCODER_H__
