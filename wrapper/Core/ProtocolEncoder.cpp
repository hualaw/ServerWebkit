#include "ProtocolEncoder.h"
#include "JobProcessor.h"
#include "SWLogger.h"

using namespace std;
using namespace WebCore;

extern SWLogger gLog;
extern Log4cplusWrapper *errLogWp;
extern Log4cplusWrapper *dbgLogWp;
extern Log4cplusWrapper *runLogWp;

ProtocolEncoder::ProtocolEncoder(SWSettings * pSettings, SWClientSocket *socket, WebkitWrapper *webkitWrapper, JobProcessor *jp, QObject * parent /*= 0*/)
:QObject(parent), pClientSocket(socket), webkitWrapper(webkitWrapper), buffer(NULL), pSettings(pSettings), jp(jp), isEncodeFinished(false)
{
    qDebug() << "webpage is " << webkitWrapper->webPage();
	connect(webkitWrapper->webPage(), SIGNAL(encodeFinished()), this, SLOT(doEncode()));
	connect(webkitWrapper->webPage(), SIGNAL(loadFinished(bool)), this, SLOT(excptDoEncode(bool)));
}

ProtocolEncoder::~ProtocolEncoder()
{
    qDebug() << "ProtocolEncoder's destruction function is called";
    if(buffer != NULL)
	{
		delete buffer;
		buffer = NULL;
	}
}

void ProtocolEncoder::response()
{
	qint64 buf_size = 0;
	buffer = getSendBuffer(buf_size);
#if 0
    QFile file("last_send_buf.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "open write file failed!";
    }
    else
    {
        qDebug() << "open write file success! response's length is " << buf_size;
        int written_size = file.write(buffer, buf_size);
        qDebug() << "written_size is " << written_size;
    }
#endif

#if 0
    QFile file("last_send_buf.txt");
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "open read file failed!";
    }
    else
    {
        QDataStream in(&file);
        int filesize = file.size();
        buffer = new char[filesize+1];
        int readRet = in.readRawData(buffer, filesize);
        buf_size = filesize;
        qDebug() << "read " << filesize << " bytes, result is " << readRet;
    }
#endif
	qint64 write_bytes = pClientSocket->write(buffer, buf_size);
#if 0
    bool flush_ret  = pClientSocket->flush();
    if(flush_ret)
    {
		gLog.log("DEBUG", "flush socket data success!");
    }
    else
    {
		gLog.log("DEBUG", "flush socket data failed!");
    }
#endif

	if(write_bytes > 0)
	{
		LOG4CPLUS_DEBUG(dbgLogWp->GetLogger(), "send response success, write " << write_bytes << " bytes to client");
	}
	else
    {
        gLog.log("DEBUG", "write response failed");
        gLog.log("ERROR", "write response failed");
	}

    jp->clearParams();
}

char * ProtocolEncoder::getSendBuffer(qint64 &buf_size)
{
    LOG4CPLUS_DEBUG(dbgLogWp->GetLogger(), "pResponse's length is " << pResponse->length);

    int newcomer_size = pResponse->length;
    char *buf = new char[newcomer_size + 10];
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

	return buf;
}

void ProtocolEncoder::doEncode()
{
    //indicate webkit has been encoded
    isEncodeFinished = true;

	pRequest = webkitWrapper->getRequest();
	pResponse = webkitWrapper->getResponse();

    //for test
#if 0
    QFile file("out.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "open write file failed!";
    }
    else
    {
        qDebug() << "open write file success! response's length is " << pResponse->length;
        file.write(pResponse->buf, pResponse->length);
    }
#endif
    //test end

    response();
}

//status: ok is true means load successfully, false mean load failed
void ProtocolEncoder::excptDoEncode(bool ok)
{
    //only handle ok is failed condition, true's ok is handled by doEncode()
    qDebug() << "enter into excptDoEncode(bool ok) slot";
    if(!ok)
    {
        qDebug() << "In excptDoEncode(bool ok) slot, ok is false, load current url failed!";
        jp->clearParams();
    }
    else
    {
        qDebug() << "In excptDoEncode(bool ok) slot, ok is true, load current url successfully!";
    }
}
