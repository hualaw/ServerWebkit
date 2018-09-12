#ifndef _SERVER_WEBKIT_WRAPPER_SENDPACKET_H_
#define _SERVER_WEBKIT_WRAPPER_SENDPACKET_H_

#include <QObject>
#include "SWGlobal.h"

class SendPacket: public QObject
{
	Q_OBJECT

public:
	SendPacket(QObject * parent = 0);
	~SendPacket();

public slots:
    void start();

private:
	void response(int pid);
	char * getSendBuffer(qint64 &, int);
    int sendAll(int socket, const char * buf, int len);
    void buildErrorResponse(int pid);

private:
	char *buffer;
};


#endif // _SERVER_WEBKIT_WRAPPER_SENDPACKET_H_
