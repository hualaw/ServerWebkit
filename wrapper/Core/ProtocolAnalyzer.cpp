#include <QDateTime>
#include <QThread>
#include <QDebug>
#include <iostream>

#include "qwebframe.h"
#include "ProtocolAnalyzer.h"
#include "RequestBuffer.h"
#include "SWUtil.h"
#include "JobProcessor.h"

using namespace std;

extern SWLogger gLog;
extern Log4cplusWrapper *errLogWp;
extern Log4cplusWrapper *dbgLogWp;
extern Log4cplusWrapper *runLogWp;
extern metaData **md;

ProtocolAnalyzer::ProtocolAnalyzer(SWClientSocket *socket, WebkitWrapper *webkit, JobProcessor *jp, int pid, QObject * parent /*= 0*/ )
:QObject(parent), pClientSocket(socket), birthTime(QDateTime::currentMSecsSinceEpoch()), state(NOT_STARTED), webkit(webkit), jp(jp), pid(pid)
{/*{{{*/
    pRequest = NULL;
    pResponse = NULL;
}/*}}}*/

ProtocolAnalyzer::~ProtocolAnalyzer()
{/*{{{*/
    qDebug() << "ProtocolAnalyzer destruction function is called";
}/*}}}*/

// Analyze protocol data received
int ProtocolAnalyzer::doAnalysis()
{/*{{{*/
    qDebug() << "enter ProtocolAnalyzer::doAnalysis()";
    qDebug() << "state: " << state;
	const char* data = pClientSocket->getReceivedData();
	int dataLength = pClientSocket->getDataSize();
    qDebug() << "data lenght: " << dataLength;
    qDebug() << "data: " << data;
    DEBUG_INFO("first three chars: %d, %d, %d", data[0], data[1], data[2]);
    DEBUG_INFO("last three chars: %d, %d, %d", data[dataLength-3], data[dataLength-2], data[dataLength-1]);

	int contentLength = 0;
	int contentPos = 0;
	int endingPos = 0;
    QByteArray content;

	switch(state)
	{
	case NOT_STARTED:
		state = WAIT_FOR_BEGINNING;

	case WAIT_FOR_BEGINNING:
        DEBUG_INFO("enter WAIT_FOR_BEGINNING");
		DEBUG_INFO("BEGINNING_SIZE:%d, INT_SIZE:%d, total:%d", BEGINNING_SIZE, INT_SIZE, BEGINNING_SIZE + INT_SIZE);
        // Wait for protocol header
		if (data == NULL || dataLength < BEGINNING_SIZE + INT_SIZE)
		{
            errorExit(QString("data is NULL or dataLength %1 < 3 + 4").arg(dataLength));
			return -1;
		}
        DEBUG_INFO("data[0]=%d, data[1]=%d, data[2]=%d", data[0], data[1], data[2]);
		// Protocol beginning must match our rule
		if (data[0] != 35 || data[1] != 42 || data[2] != 2)
		{
			errorExit(QString("illegal protocol beginning, first byte: %1, second byte: %2, third byte: %3").arg(data[0]).arg(data[1]).arg(data[2]));
			return -1;
		}

		// Get length of protocol content
		contentLength = make_int(data, BEGINNING_SIZE);
        DEBUG_INFO("contentLength:%d", contentLength);
		if (contentLength <= 0)
		{
			errorExit("empty content");
			return -1;
		}

		state = WAIT_FOR_FULL_DATA;

	case WAIT_FOR_FULL_DATA:
        DEBUG_INFO("enter WAIT_FOR_FULL_DATA");
        DEBUG_INFO("dataLength:%d, BEGINNING_SIZE + INT_SIZE + contentLength + ENDING_SIZE:%d", dataLength, BEGINNING_SIZE + INT_SIZE + contentLength + ENDING_SIZE);
		// Wait for full protocol data.
		if (dataLength < BEGINNING_SIZE + INT_SIZE + contentLength + ENDING_SIZE)
		{
			errorExit(QString("dataLength %1 < BEGINNING_SIZE %2: %1, second byte: %2, third byte: %3").arg(data[0]).arg(data[1]).arg(data[2]));
			return -1;
		}

		// Protocol ending must match our rule
		endingPos = dataLength - ENDING_SIZE;
        DEBUG_INFO("endingPos:%d", endingPos);
        DEBUG_INFO("data[endingPos]:%d,data[endingPos + 1]:%d,data[endingPos + 2]:%d", data[endingPos],data[endingPos + 1],data[endingPos + 2]);
		if (data[endingPos] != 35 || data[endingPos + 1] != 42 || data[endingPos + 2] != 3)
		{
			errorExit("illegal protocol ending");
			return -1;
		}

		// Fetch and process protocol content
		contentPos = BEGINNING_SIZE + INT_SIZE;
        content.append(data + contentPos, contentLength);
        return processContent(content);

		break;

	default:
		errorExit("unreachable state when doing analysis");
        return -1;
		break;
	}
    qDebug() << "leave ProtocolAnalyzer::doAnalysis()";
    return 0;
}/*}}}*/

// Exit analyzer when error occurred
void ProtocolAnalyzer::errorExit( QString errorMsg)
{/*{{{*/
	if (!errorMsg.isNull() && !errorMsg.isEmpty())
	{
        //LOG4CPLUS_ERROR(errLogWp->GetLogger(), "ProtocolAnalyzer error, socket: " << pClientSocket->socketDescriptor() << ", errmsg: " << errorMsg.toStdString());
        LOG4CPLUS_ERROR(errLogWp->GetLogger(), "ProtocolAnalyzer error, errmsg: " << errorMsg.toStdString());
	}

    if( pClientSocket )
    {
        //pClientSocket->close();
        qDebug() << "ProtocolAnalyzer::errorExit close socket";
    }

    if( pRequest != NULL )
    {
        delete pRequest;
        pRequest = NULL;
        qDebug() << "pRequest is free in ProtocolAnalyzer::errorExit";
    }

    if( pResponse != NULL )
    {
        if(pResponse->length > 0 && pResponse->buf != NULL)
        {
            delete pResponse->buf;
            pResponse->buf = NULL;
            qDebug() << "pResponse's buf is free in ProtocolAnalyzer::errorExit";
        }
        pResponse->length = 0;
        delete pResponse;
        pResponse = NULL;
        qDebug() << "pResponse is free in ProtocolAnalyzer::errorExit";
    }
}/*}}}*/

// Process protocol content
int ProtocolAnalyzer::processContent( QByteArray content)
{/*{{{*/
    DEBUG_ENTER();
	if (content.isNull() || content.isEmpty())
	{
		errorExit("empty content");
		return -1;
	}

	state = PROCESSING_CONTENT;

	// Process content and make the request object
	pRequest = makeRequest(content);
    if( pRequest == NULL )
    {
        errorExit("pRequest create failed");
        return -1;
    }

    DEBUG_INFO("after makeRequest(), screen width is:%d", pRequest->getScreenWidth());

    pResponse = new QhEventResponse();
    if( pResponse == NULL )
    {
		errorExit("new QhEventResponse() return NULL");
		return -1;
    }
    pResponse->length = 0;
    pResponse->buf = NULL;

    //add pResponse to global metaData Array
    md[pid]->resp = pResponse;

	// Handle the request
	state = HANDLING_REQUEST;
    DEBUG_INFO("call hanldeRequest");
	handleRequest(pRequest, pResponse);

	// Succeed processing request
	state = COMPLETED;
    DEBUG_LEAVE();
    return 0;
}/*}}}*/

// Make a request object from content received
QhEventStruct*  ProtocolAnalyzer::makeRequest( QByteArray content )
{/*{{{*/
    DEBUG_ENTER();

	QhEventStruct *pRequest = new QhEventStruct();
	if (pRequest == NULL)
	{
		errorExit("new QhEventStruct return NULL");
		return NULL;
	}
    qDebug() << "QhEventStruct in ProtocolAnalyzer::makeRequest() is " << pRequest;

	const char *pData = content.data();

    qDebug() << "rb length is: " << content.length();
	RequestBuffer rb(content.length(), pData);

	pRequest->setOS("undefined");

	// Fetch and process protocol header
	QString header = rb.read_header();
	if (header.isNull() || header.isEmpty()) {
		errorExit(QString("header length is error: %1").arg(header));
		return NULL;
	}
	int result = processHeader(header, pRequest);
    if( result != 0 )
    {
		errorExit("processHeader() return -1");
		return NULL;
    }

	// Fetch request type
	pRequest->setType(rb.read_int());
	if (pRequest->getType()!= EventType::OPEN_URL 
		&& pRequest->getType()!= EventType::COMMIT)
	{
		errorExit(QString("Unexpected Event Type from the user! Event Type=%1").arg(pRequest->getType()));
		return NULL;
	}

	pRequest->setTm(rb.read_int());
	rb.read_int();//screen_width
	rb.read_int();//screen_height
	
	rb.read_string();//get userid

	rb.read_string(); //get transactionid
	rb.read_string(); //getpageid

	rb.read_string();//get font

	int num = 0;
	QString strTemp = rb.read_string();

    DEBUG_INFO("%s, url:%s", qPrintable(QDateTime::currentDateTime().toLocalTime().toString("yyyy-MM-dd hh:mm:ss")), qPrintable(strTemp));
	pRequest->setURL(strTemp.toUtf8().data());

	if (pRequest->getType() == EventType::COMMIT)
	{
		num = rb.read_int();
        qDebug() << "Commit Request type, total " << num << "CtrlEntry ";
		for (int i=0; i<num; ++i)
		{
			QhCtrlEntry ce;
            int id = rb.read_int();
			ce.setId(id);
			strTemp = rb.read_string();
			ce.setContent(strTemp.toUtf8().data());
			vector<QhCtrlEntry>* controls = pRequest->getControls();
            controls->push_back(ce);
            qDebug() << "Commit Request type, CtrlEntry " << i << " id:" << id <<", content:" << strTemp;
		}
        qDebug() << "rb pointer size is: " << rb.get_ptr_size();

        int zid =  rb.read_int();
        qDebug() << "zid is: " << zid;
		pRequest->setZid(zid);

		pRequest->setX(rb.read_int());
		pRequest->setY(rb.read_int());
	}

    DEBUG_LEAVE();
	return pRequest;
}/*}}}*/

// Process protocol header and exit if pid is out of range
int ProtocolAnalyzer::processHeader( QString header, QhEventStruct* pRequest )
{/*{{{*/
    DEBUG_ENTER();
	if (header.isNull() || header.isEmpty())
	{
        errorExit("header is NULL or Empty!");
		return -1;
	}

    DEBUG_INFO("header:%s", qPrintable(header));

	int start = 0;
	int tb = 102 * 3;
	QString key;
	QString value;

	while (start != -1)
	{
		readPair(header, key, value, start, tb);

		//for last key, move if statement here
        if (start == -1)
        {
            DEBUG_INFO("reach -1, break");
            break;
        }

        DEBUG_INFO("header >> %s=%s", key.toUtf8().data(), value.toUtf8().data());
        if (key == CTRL_OS_KEY)
        {
            DEBUG_INFO("set CTRL_OS_KEY");
			pRequest->setOS(value.toUtf8().data());
            if (value == "Android")
            {
                DEBUG_INFO("set Mx 13");
				pRequest->setMx(13);
				//tb = 582;
			}
        }
        //add by Tot 12.06.10 for debug webkit wrapper ------------------
        else if (key == CTRL_UID_KEY)
        {
            DEBUG_INFO("set CTRL_UID_KEY");
			pRequest->setUserID(value.toUtf8().data());
		} 
        else if(key == CTRL_TID_KEY)
        {
            DEBUG_INFO("set CTRL_TID_KEY");
			pRequest->setTransactionID(value.toUtf8().data());
		}
        else if (key == CTRL_PID_KEY)
        {
            DEBUG_INFO("set CTRL_PID_KEY");
			pRequest->setPageID(value.toUtf8().data());
		}
        else if (key == CTRL_L_KEY)
        {
            DEBUG_INFO("set CTRL_L_KEY");
			pRequest->setL(value.toInt());
		}
        else if (key == CTRL_SCREEN_WIDTH_KEY)
        {
            DEBUG_INFO("set CTRL_SCREEN_WIDTH_KEY");
			pRequest->setScreenWidth(value.toInt());
		}
        else if (key == CTRL_SCREEN_HEIGHT_KEY)
        {
            DEBUG_INFO("set CTRL_SCREEN_HEIGHT_KEY");
			pRequest->setScreenHeight(value.toInt());
		}
        else if (key == CTRL_LS_KEY)
        {
            DEBUG_INFO("set CTRL_LS_KEY");
			pRequest->setLs(value.toInt());
		}
        else if (key == CTRL_PQ_KEY)
        {
            DEBUG_INFO("set CTRL_PQ_KEY");
			pRequest->setPq(value.toInt());
		}
        else if (key == CTRL_SEQ_KEY)
        {
            DEBUG_INFO("set CTRL_SEQ_KEY");
			pRequest->setSeq(value.toUtf8().data());
		}
        else if (key == CTRL_DELETE_COOKIE_KEY)
        {
            DEBUG_INFO("set CTRL_DELETE_COOKIE_KEY");
			pRequest->setDeleteCookies(value.toUtf8().data());
		}
        else if (key == CTRL_PV_KEY)
        {
            DEBUG_INFO("set CTRL_PV_KEY");
			pRequest->setPv(atof(value.toUtf8().data()));
		}

        if (key == CTRL_F_KEY)
        {
            DEBUG_INFO("set CTRL_F_KEY");
			QString f = value;
            vecInt *zfont = pRequest->getCliFonts();
			for (int i = 0; i < f.length(); i++)
				zfont->push_back((int)(f.toUtf8().data()[i]));
			pRequest->setFontWidth((*zfont)[1]);
        }
        else if (key == "Cache-Control")
        {
            DEBUG_INFO("set Redirect -2");
			pRequest->setRedirect(-2);
        }
        else
        {
            DEBUG_INFO("all headers add into xHeader");
            vecPairString *xHeader = pRequest->getXHeader();
            std::pair<std::string, std::string> oneHeader;
            oneHeader.first = std::string(key.toUtf8().data());
            oneHeader.second = std::string(value.toUtf8().data());
            xHeader->push_back(oneHeader);
        }
    }
    DEBUG_LEAVE();
    return 0;
}/*}}}*/

void ProtocolAnalyzer::readPair(QString &src, QString &key, QString &value, int &start, int tb)
{/*{{{*/
    DEBUG_ENTER();

	int pos = 0;
	pos = src.indexOf(": ", start);
	if ((pos <= start) || (pos == -1))
	{
		start = -1;
		return;
	}
	key = src.mid(start, pos - start);
	start = pos + 2;
	if (key == CTRL_F_KEY)
		pos = start + tb;
	else
		pos = src.indexOf("\r\n", start);
	if (pos == -1)
	{
		value = src.mid(start);
		start = -1;
	}
	else
	{
		value = src.mid(start, pos - start);
		start = pos + 2;
	}
    DEBUG_LEAVE();
}/*}}}*/

void ProtocolAnalyzer::handleRequest(QhEventStruct* pRequest, QhEventResponse *pResponse)
{/*{{{*/
    DEBUG_ENTER();
    DEBUG_INFO("pRequest:%x, pResponse:%x", pRequest, pResponse);
	char* timeStr = QDateTime::currentDateTime().toLocalTime().toString("yyyy-MM-dd hh:mm:ss").toUtf8().data();
    LOG4CPLUS_DEBUG(dbgLogWp->GetLogger(), "******Got a request from user, time is " << timeStr);
    DEBUG_INFO("******Got a request from user, time is :%s", timeStr);
    webkit->handleRequest(pRequest, pResponse);
    DEBUG_LEAVE();
}/*}}}*/
