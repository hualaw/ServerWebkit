#ifndef _SERVER_WEBKIT_WRAPPER_CLIENT_SOCKET_H__
#define _SERVER_WEBKIT_WRAPPER_CLIENT_SOCKET_H__

#include <QTcpSocket>
#include <QAbstractSocket>
#include <QWaitCondition>
#include <QReadWriteLock>

const static uint BUFFER_SIZE = 1024 * 8;

class SWClientSocket : public QTcpSocket
{
Q_OBJECT

public:
	SWClientSocket(QObject* parent = 0);
	~SWClientSocket();

	const char* getReceivedData();
	int getDataSize();

signals:
	void readyForAnalysis();

protected slots:
	void dataReceived();
	void socketErrorHandle(QAbstractSocket::SocketError error);
    void customStateChanged(QAbstractSocket::SocketState);

public slots:
    void mySetSocketDescriptor(int, int);

private:
	QByteArray data;
    //char *buffer;
};

#endif // _SERVER_WEBKIT_WRAPPER_CLIENT_SOCKET_H__

