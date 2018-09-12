#include <QApplication>
#include "SWServer.h"
#include "JobProcessor.h"

extern SWLogger gLog;
extern Log4cplusWrapper *errLogWp;
extern Log4cplusWrapper *dbgLogWp;
extern Log4cplusWrapper *runLogWp;
extern WebkitWrapper **ww;
extern metaData **md;
extern SWSettings settings;
extern JobProcessor *jp;

SWServer::SWServer(QString port, QObject *parent)
:port(port), QTcpServer(parent)
{
}

void SWServer::start()
{
	bool listenRet = listen(QHostAddress::Any, (quint16)(port.toInt()));
    if(!listenRet)
    {
        gLog.log("ERROR", QString("SWServer listen localhost port %1 failed!").arg(port));
        QThread::currentThread()->exit(-1);
    }
}

// Clear all client sockets
SWServer::~SWServer()
{
}

void SWServer::cleanupGlobalParams()
{
    //free WebkitWrapper Pointer Array
    if( ww != NULL )
    {
        for(int i = 0; i< settings.getWindowCount(); i++)
        {
            (ww[i])->deleteLater();
            ww[i] = NULL;
            qDebug() << "webkitwrapper array ["<< i <<"] free";
        }
        delete []ww;
        ww = NULL;
        qDebug() << "webkitwrapper array self memory free";
    }

    if(md != NULL)
    {
        for(int i = 0; i< settings.getWindowCount(); i++)
        {
            delete md[i];
            qDebug() << "metadata array ["<< i <<"] free";
        }

        delete []md;
        qDebug() << "metadata array self memory free";
    }
    if( jp != NULL )
    {
        jp->deleteLater();
        jp = NULL;
        qDebug() << "jp memory free";
    }
    qDebug() << "SWServer's cleanupGlobalParams() called over";
}

// Setup client socket for protocol thread
void SWServer::incomingConnection(int socketDescriptor)
{
    qDebug() << "-------enter incomingConnection, socketDescriptor is:" << socketDescriptor << "---------";
    //create clientSocket
    clientSocket = new SWClientSocket();
	QObject::connect(clientSocket, SIGNAL(disconnected()), this, SLOT(deleteSocket()));
	QObject::connect(clientSocket, SIGNAL(readyForAnalysis()), this, SLOT(updateMetadata()));

    int sendBufferSize = settings.getSendBufferSize();
	clientSocket->mySetSocketDescriptor(socketDescriptor, sendBufferSize);
}

void SWServer::deleteSocket()
{
	if( clientSocket ) clientSocket->deleteLater();
    clientSocket = NULL;
	qDebug() << "socket is free and deleted!";
}

void SWServer::updateMetadata()
{
    qDebug() << "enter SWServer::updateMetadata()";

    std::string s(clientSocket->getReceivedData(), clientSocket->getDataSize());
    qDebug() << QString("receive data: %1").arg(s.c_str());

    size_t posStart = s.find("x-zmctrl-pid: ", 4) + 14;
    size_t posEnd = s.find("\r\n", posStart);
    qDebug() << QString("posStart: %1").arg(posStart);
    qDebug() << QString("posEnd: %1").arg(posEnd);

    if( posStart == 13 || posEnd == -1 )
    {
        //free clientSocket
        if( clientSocket )
        {
            clientSocket->close();
            qDebug() << "SWServer::updateMetadata pid-pos exception";
        }

        gLog.log("ERROR", QString("pid length exception, pidStart:%1, pidEnd:%2").arg(posStart).arg(posEnd));
        return;
    }

    int pid_length = posEnd - posStart;
    int pid = atoi(s.substr(posStart, posEnd - posStart).c_str()) - 1;
    
    int wc = settings.getWindowCount();
    qDebug() << QString("pid: %1").arg(pid);
    qDebug() << QString("wc: %1").arg(wc);
    if( pid >= 0 && pid < wc )
    {
        qDebug() << "set state: ACCEPTED";
        md[pid]->state = ACCEPTED;
        md[pid]->socket = clientSocket;

        emit readyForProcess(pid);
    }
    else
    {
        qDebug() << "close socket";
        //free clientSocket
        if( clientSocket )
        {
            clientSocket->close();
            qDebug() << "SWServer::updateMetadata pid-value exception";
        }
        gLog.log("ERROR", QString("pid %1 is out of range(0,%2)!").arg(pid).arg(wc));
    }
    qDebug() << "leave SWServer::deleteSocket()";
}
