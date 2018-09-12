#include <QDebug>
#include <QThread>
#include <QFile>
#include <Assertions.h>

#include "SWClientSocket.h"
extern "C"
{
    #include <sys/types.h>
    #include <sys/socket.h>
}

SWClientSocket::SWClientSocket(QObject* parent)
{
	//data.reserve(BUFFER_SIZE);
    //buffer = NULL;
    //buffer = new char [BUFFER_SIZE];
    //if( buffer == NULL )
    //{
    //    qDebug() << "SWClientSocket new failed";
    //}
	qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
	qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");

	connect(this,SIGNAL(readyRead()),this,SLOT(dataReceived()));
	connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketErrorHandle(QAbstractSocket::SocketError)));
	//connect(this,SIGNAL(disconnected()),this,SLOT(doDisconnected()));
    connect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(customStateChanged(QAbstractSocket::SocketState)));
}

void SWClientSocket::customStateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << "socket state changed, state:" << state;
}

SWClientSocket::~SWClientSocket()
{
    //data.clear();
    //if( buffer != NULL)
    //{
    //    delete buffer;
    //    buffer = NULL;
    //}
    qDebug() << "SWClientSocket destruction function is called";
}

// Collect incoming data
void SWClientSocket::dataReceived()
{
    qDebug() << "enter SWClientSocket::dataReceived()";

	char buffer[BUFFER_SIZE] = {};
	qDebug() << " emit readyForAnalysis() signal thread is: " << QThread::currentThreadId();
    data.clear();
    qDebug() << "available bytes: " << bytesAvailable();
	while(bytesAvailable() > 0)
	{
		qint64 length = read(buffer, BUFFER_SIZE);
        qDebug() << "finish read data length: " << length;
		if (length > 0)
		{
			// Copy once
			data.append(buffer, length);
		}
	}
    QString dataString(data);
    qDebug() << "total lenght: " << data.length();
    qDebug() << "receive data: " << dataString;

    /*
    //write data to file
    QFile file("in.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "open write file failed!";
    }
    else
    {
        qDebug() << "open write file success!";
        file.write(data);
    }
    */

    //write end
    if(data.size() > 10)
    {
        emit readyForAnalysis();
    }
    else
    {
        close();
        qDebug() << "socket data size is smaller than 10, close socket";
    }
}

// Return received data as a byte array
const char* SWClientSocket::getReceivedData()
{
    return data.data();
}

int SWClientSocket::getDataSize()
{
    qDebug() << "input packet size is: " << data.size();
	return data.size();
}

void SWClientSocket::socketErrorHandle(QAbstractSocket::SocketError error)
{
	qDebug() << " socket error occur, errstring:" << errorString() << ", errno:" << error;
}

void SWClientSocket::mySetSocketDescriptor(int socketD, int sendBufferSize)
{
    DEBUG_ENTER("socketID:%d, sendBufSize:%d", socketD, sendBufferSize);
    setSocketDescriptor(socketD);

    qDebug() << "sendBufferSize is " << sendBufferSize;
    setsockopt(socketD, SOL_SOCKET, SO_SNDBUF, &sendBufferSize, sizeof(int));
    qDebug() << "leave SWClientSocket::mySetSocketDescriptor()";
    DEBUG_LEAVE();
}
