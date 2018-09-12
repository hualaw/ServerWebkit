#ifndef _SERVER_WEBKIT_WRAPPER_SERVER_H__
#define _SERVER_WEBKIT_WRAPPER_SERVER_H__

#include <QTcpServer>
#include "SWGlobal.h"

class SWServer: public QTcpServer
{
	Q_OBJECT

public:
	SWServer(QString port, QObject *parent = 0);
	~SWServer();

signals:
    void readyForProcess(int);

public slots:
    void start();
	void deleteSocket();
    void updateMetadata();
    void cleanupGlobalParams();

protected:
	void incomingConnection(int socketDescriptor);

private:
	QString port; //listening port
    SWClientSocket *clientSocket;
};

#endif // _SERVER_WEBKIT_WRAPPER_SERVER_H__
