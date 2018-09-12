#include "SendPacket.h"
#include "SWQThread.h"
#include "BinaryStream.h"
#include <Assertions.h>

extern "C"
{
    #include <sys/types.h>
    #include <sys/socket.h>
}

extern SWLogger gLog;
extern Log4cplusWrapper *errLogWp;
extern Log4cplusWrapper *dbgLogWp;
extern Log4cplusWrapper *runLogWp;
extern WebkitWrapper **ww;
extern metaData **md;
extern SWSettings settings;

SendPacket::SendPacket(QObject * parent /*= 0*/)
{
    buffer = NULL;
}

SendPacket::~SendPacket()
{
    qDebug() << "SendPacket's destruction function is called";
    if(buffer != NULL)
	{
		delete buffer;
		buffer = NULL;
	}
}

void SendPacket::start()
{
    DEBUG_ENTER();
    int i = 0;
    int wc = settings.getWindowCount();
    while(1)
    {
        for(i=0; i< wc; i++)
        {
            DEBUG_INFO("handle i:%d", i);
            switch(md[i]->state)
            {
            case ENCODED_SUCCESS:
                DEBUG_INFO("enter ENCODED_SUCCESS");
                response(i);
                md[i]->state = SENT; //modify state to stop repeatedly sent response
                break;

            case ENCODED_FAILED:
                DEBUG_INFO("enter ENCODED_FAILED");
                md[i]->state = SENT; //modify state to stop repeatedly sent response
                QhEventStruct *es = ww[i]->getRequest();

                //transparently send webkit main resource's error code
                qDebug() << "es->getStatus is: " << es->getStatus(); 
                if( es && es->getStatus() != 0 && es->getStatus() != 200 )
                {
                    buildErrorResponse(i);
                    response(i);
                }
                else
                {
                    if( md[i]->socket )
                    {
                        //close that not timeout situation 's encode failed
                        md[i]->socket->close();
                        md[i]->socket = NULL;
                        qDebug() << "SendPacket::start() close socket";
                    }
                }
                ww[i]->clearParams(); //add to free webkitwrapper's params when encode failed
                break;
            }
        }

        /*
         *change as 1second for debug
         *must change back to 10 milsecond
        */
        SWQThread::myUsleep(1000000);
    }
    DEBUG_LEAVE();
}

void SendPacket::response(int pid)
{
    DEBUG_ENTER("socket: %x", md[pid]->socket);

    if( md[pid]->socket == NULL)
    {
        gLog.log("ERROR", "when send response, socket is NULL");
        DEBUG_INFO("socket is NULL");
        return;
    }

	qint64 buf_size = 0;
    int socketD = md[pid]->socket->socketDescriptor();

	buffer = getSendBuffer(buf_size, pid);
	int ret = sendAll(socketD, buffer, buf_size);
    int write_bytes = buf_size;

	if(ret ==  0)
	{
		LOG4CPLUS_DEBUG(dbgLogWp->GetLogger(), "send response success, write " << write_bytes << " bytes to client");
        qDebug() << "send response success, write " << write_bytes << " bytes to client";
        DEBUG_INFO("send response success");
	}
	else
    {
		LOG4CPLUS_DEBUG(dbgLogWp->GetLogger(), "write response failed");
        qDebug() << "write response failed";
        DEBUG_INFO("send response failed.");
    }

    //should close socket at anytime
    if( md[pid]->socket && md[pid]->socket->isOpen())
    {
        md[pid]->socket->close();
        DEBUG_INFO("close socket.");
    }
    md[pid]->socket = NULL;
    
    //free buffer
    if(buffer != NULL)
    {
        delete []buffer;
        buffer = NULL;
    }

    ww[pid]->clearParams();
    DEBUG_LEAVE();
}

char * SendPacket::getSendBuffer(qint64 &buf_size, int pid)
{
    DEBUG_ENTER();

    QhEventResponse *pResponse = md[pid]->resp;

    LOG4CPLUS_DEBUG(dbgLogWp->GetLogger(), "pResponse's length is " << pResponse->length);
    DEBUG_INFO("pResponse's length is:%d", pResponse->length);
    DEBUG_INFO("pResponse's data is:%s", pResponse->buf+4);

    int newcomer_size = pResponse->length;
    char *buf = NULL;
    buf = new char[newcomer_size + 10];
    if( buf == NULL )
    {
        qDebug() << "SendPacket::getSendBuffer, new create " << (newcomer_size+10) << " bytes buf failed";
    }
    memset(buf, '\0', newcomer_size + 10);
    buf[0] = 35;
    buf[1] = 42;
    buf[2] = 2;
    buf[3] = ((newcomer_size>>24) & 0xFF);
    buf[4] = ((newcomer_size>>16)  & 0xFF);
    buf[5] = ((newcomer_size>>8)  & 0xFF);
    buf[6] = (newcomer_size  & 0xFF);
    for (int j = 0; j < newcomer_size; j++)
        buf[j + 7] = pResponse->buf[j];
    buf[newcomer_size + 7] = 35;
    buf[newcomer_size + 8] = 42;
    buf[newcomer_size + 9] = 3;

	buf_size = newcomer_size + 10;
    DEBUG_LEAVE("ready for sent data length:%d", buf_size);

	return buf;
}

int SendPacket::sendAll(int socket, const char * buf, int len)
{
    DEBUG_ENTER("socket:%d, data length:%d, data:%s", socket, len, buf);

    int iByteTotal = 0;
    char * ps = (char *)buf;
    int iRemainBufLen = len;

    while(iRemainBufLen > 0)
    {
        DEBUG_INFO("iRemainBufLen:%d, sendOutLen:%d", iRemainBufLen, iByteTotal);
        iByteTotal = send(socket, ps, iRemainBufLen, 0);

        if (iByteTotal<0)
        {
            DEBUG_INFO("send data to socket error.");
            return -1;
        }
        else
        {
            ps += iByteTotal;
            iRemainBufLen -= iByteTotal;
        }
        SWQThread::myUsleep(10000);
    }
    DEBUG_LEAVE();
    return 0;
}

void SendPacket::buildErrorResponse(int pid)
{
    int i;
    QhEventResponse *pResponse = md[pid]->resp;
    QhEventStruct *es = ww[pid]->getRequest();

    //free pResponse first
    if( pResponse->buf != NULL )
    {
        delete pResponse->buf;
        pResponse->buf = NULL;
    }
    pResponse->length = 0;

    //write error-code to pResponse
    BinaryStream m_prehead;
    m_prehead.write_int(0); // to modify later
    m_prehead.write_bytes("x-zmctrl-Status: ");
    char tmpBuf[32];
    sprintf(tmpBuf, "%d", es->getStatus());
    m_prehead.write_bytes(tmpBuf);
    m_prehead.write_bytes("\r\n");

    int headLen = es->getXHeader()->size();
    const char *tmpStr;
    for (i = 0; i < headLen; i++) {
        tmpStr = (*es->getXHeader())[i].first.c_str();
        m_prehead.write_bytes(tmpStr);
        m_prehead.write_bytes(": ");
        tmpStr = (*es->getXHeader())[i].second.c_str();
        m_prehead.write_bytes(tmpStr);
        m_prehead.write_bytes("\r\n");
    }
    headLen = es->getWHeader()->size();
    for (i = 0; i < headLen; i++) {
        tmpStr = (*es->getWHeader())[i].first.c_str();
        m_prehead.write_bytes(tmpStr);
        m_prehead.write_bytes(": ");
        tmpStr = (*es->getWHeader())[i].second.c_str();
        m_prehead.write_bytes(tmpStr);
        m_prehead.write_bytes("\r\n");
    }
    m_prehead.set_int(0, m_prehead.size() - 4); //write length

    int length = m_prehead.size() + 1;

    qDebug() << "response's buf size is: "<< length;

    pResponse->buf = new char[length];
    if( pResponse->buf )
    {
        for(i=0; i<m_prehead.size(); i++) pResponse->buf[i] = m_prehead[i]; 

        pResponse->buf[length -1] = '\0';
        pResponse->length = length;

        qDebug() << "enter SendPacket::buildErrorResponse(), response buf is: " << pResponse->buf;
    }
    else
    {
        gLog.log("ERROR", "allocate pResponse->buf failed");
    }
}
