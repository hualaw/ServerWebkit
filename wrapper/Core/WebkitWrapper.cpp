#include <iostream>

#include "config.h"
#include "Frame.h"
#include "qwebframe.h"
#include "WebkitWrapper.h"
#include "JobProcessor.h"
#include "SendPacket.h"

using namespace std;
namespace WebCore {
    class Frame;
    class Settings;
}

extern SWLogger gLog;
extern Log4cplusWrapper *errLogWp;
extern Log4cplusWrapper *dbgLogWp;
extern Log4cplusWrapper *runLogWp;
extern metaData **md;

WebkitWrapper::WebkitWrapper(QNetworkProxy proxy, QObject *parent)
:QObject(parent), m_pRequest(NULL), m_pResponse(NULL), pid(0), jp(NULL)
{
	//m_pMainPage = new QWebPage(this);
	m_pPage = new SWWebPage(this);

	//add proxy
	//m_pPage->networkAccessManager()->setProxy(proxy);

	// clean up cache by resetting quota.
	qint64 quota = webPage()->settings()->offlineWebApplicationCacheQuota();
	webPage()->settings()->setOfflineWebApplicationCacheQuota(quota);

	connect(m_pPage, SIGNAL(encodeFinished()), this, SLOT(updateMetadataStatus()));
	connect(m_pPage, SIGNAL(loadFinished(bool)), this, SLOT(exceptUpdateMetadataStatus(bool)));
	//QObject::connect(this, SIGNAL(quit()), qApp, SLOT(quit()), Qt::QueuedConnection);
}

WebkitWrapper::~WebkitWrapper()
{
    if(m_pPage != NULL)
    {
        m_pPage->deleteLater();
        m_pPage = NULL;
    }
}

void WebkitWrapper::clearParams()
{
   qDebug() << "WebkitWrapper's destruction function is called"; 

   //open blow comment will result in QhEventStruct->getCliFont() core dump in webkit
   /*
	if(m_pRequest != NULL)
	{
		delete m_pRequest;
		m_pRequest = NULL;
	}
    */

   qDebug() << "m_pRequest size is: "<< sizeof(m_pRequest);

    if(m_pResponse != NULL )
    {
        if(m_pResponse->length > 0 && m_pResponse->buf != NULL)
        {
            delete []m_pResponse->buf;
            m_pResponse->buf = NULL;
            qDebug() << "WebkitWrapper's destruction, pResponse's buf is free";
        }
        m_pResponse->length = 0;
        delete m_pResponse;
        m_pResponse = NULL;
        qDebug() << "WebkitWrapper's destruction, pResponse is free";
    }
}

void WebkitWrapper::open(const QUrl& url)
{
    DEBUG_ENTER();
	m_pPage->mainFrame()->load(url);
    DEBUG_LEAVE();
}

void WebkitWrapper::handleRequest(QhEventStruct* pRequest, QhEventResponse *pResponse)
{
    DEBUG_ENTER();
    if(m_pRequest != NULL)
    {
        DEBUG_INFO("try to delete m_pRequest:%x", m_pRequest);
        printf("m_pRequest is 0x%x\n", m_pRequest);
        fflush(stdout);
        delete m_pRequest;
        m_pRequest = NULL;
        qDebug() << "WebkitWrapper's pRequest is free";
    }
    
    DEBUG_INFO("pRequest:%x, pResponse:%x", pRequest, pResponse);

    pRequest->setResultFormat(ResultFormat::HTML_FORMAT);
	m_pRequest = pRequest;
    m_pResponse = pResponse;


    QWebFrame *myFrame = m_pPage->mainFrame();
    WebCore::Frame* frame = myFrame->getFrame();
    myFrame->setEventStruct(pRequest);
    myFrame->setEventResponse(pResponse);
    
    DEBUG_INFO("m_pResponse is 0x%x\n", m_pResponse);
    printf("m_pResponse is 0x%x\n", m_pResponse);
    fflush(stdout);

	if (frame) {
        DEBUG_INFO("frame is valid:%x", frame);
		WebCore::Settings* s = frame->settings();

        //clear custom settings first
        s->reset();
        m_pPage->settings()->resetFontSize(QWebSettings::DefaultFontSize);

		if (s)
		{
            DEBUG_INFO("WebCore::Settings* s is valid:%x", s);
			s->setScreenWidth(m_pRequest->getScreenWidth());
			s->setFontWidth(m_pRequest->getFontWidth());
            DEBUG_INFO("QWebSetting's set screen width is: %d", m_pRequest->getScreenWidth());
            if (m_pRequest->getOS())
            {
                DEBUG_INFO("OS is: %s", m_pRequest->getOS());
            }
            else
            {
                DEBUG_INFO("OS is null");
            }
            //DEBUG_INFO("setting's OS: %s", s->os().c_str());
            s->setOS(m_pRequest->getOS());
            DEBUG_INFO("OS is: %s", m_pRequest->getOS());

            DEBUG_INFO("L is: %d", m_pRequest->getL());
			s->setL(m_pRequest->getL());
			
			s->setTemplateFilter(m_pRequest->getTemplateFilter());
			s->setURL(m_pRequest->getURL());
			s->setPQ(m_pRequest->getPq());
			s->setLayoutAlgorithm(WebCore::Settings::kLayoutFitColumnToScreen);
			
            int font_height = m_pRequest->getCliFont(0);
            s->setFontHeight(font_height);
            DEBUG_INFO("Font vector size is: %d", m_pRequest->getCliFont(0));
			
            s->setLs(5);
		}
	}
    
    DEBUG_INFO("set page attributes");

	m_pPage->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, false);
	m_pPage->settings()->setDefaultTextEncoding(QLatin1String("GB18030"));

	if (m_pRequest->getL() == 1)
		m_pPage->settings()->setAttribute(QWebSettings::ZoomTextOnly, true);
	else
		m_pPage->settings()->setAttribute(QWebSettings::ZoomTextOnly, false);

	if (m_pRequest->getPq() == 3)
		m_pPage->settings()->setAttribute(QWebSettings::AutoLoadImages, false);
	else
		m_pPage->settings()->setAttribute(QWebSettings::AutoLoadImages, true);

    url = QUrl::fromUserInput(QString::fromUtf8(m_pRequest->getURL()));
    
    DEBUG_INFO("url:%s", qPrintable(url.toString()));

	if( m_pRequest->getType() == EventType::OPEN_URL)
	{
        DEBUG_INFO("enter OPEN_URL");
        LOG4CPLUS_DEBUG(dbgLogWp->GetLogger(), "url is: " << url.toString().toUtf8().constData());
        qDebug() << "load url is: " << url.toString().toUtf8().constData();

        //clear history
        m_pPage->clearHistory();

        DEBUG_INFO("user id:%s", m_pRequest->getUserID());
        DEBUG_INFO("pid:%s", m_pRequest->getPageID());
        DEBUG_INFO("tid:%s", m_pRequest->getTransactionID());

        //open url
        this->open(url);
	}
    else
    {
        DEBUG_INFO("enter not OPEN_URL");
        qDebug() << "Request's type is " << m_pRequest->getType();
        //m_pMainPage->handleEvent(m_pRequest, url, myFrame); 
        m_pPage->mouseClickEvent(m_pRequest, url); 
    }
    DEBUG_LEAVE();
}

void WebkitWrapper::updateMetadataStatus()
{
    DEBUG_ENTER();
    QString runlog = QString("load page success! url is: ").append(url.toString().toUtf8().constData());
    qDebug() << runlog;
    LOG4CPLUS_DEBUG(runLogWp->GetLogger(), runlog.toUtf8().data());
    DEBUG_INFO("%s", runlog.toUtf8().data());
    DEBUG_INFO("update pid:%d", pid);
    if(pid >= 0)
    {
        md[pid]->state = ENCODED_SUCCESS;
    }
    if(jp != NULL)
    {
        jp->clearParams();
    }
    DEBUG_LEAVE();
}

void WebkitWrapper::exceptUpdateMetadataStatus(bool ok)
{
    DEBUG_ENTER();
    //only handle ok is false's situation, when m_pRespose->length's is more than 10MB, return false
    if( !ok  || (m_pResponse && (m_pResponse->length < 0 || m_pResponse->length >= 10485760)) )
    {
        if( ok )
        {
            QString errlog = QString("m_pResponse->length %1 is abnormal").arg(m_pResponse->length);
            qDebug() << errlog;
            LOG4CPLUS_DEBUG(errLogWp->GetLogger(), errlog.toUtf8().data());
            DEBUG_INFO("%s", errlog.toUtf8().data());
        }

        QString runlog = QString("load page failed! url is: ").append(url.toString().toUtf8().constData());
        qDebug() << runlog;
        LOG4CPLUS_DEBUG(runLogWp->GetLogger(), runlog.toUtf8().data());
        DEBUG_INFO("%s", runlog.toUtf8().data());

        if(pid >= 0)
        {
            md[pid]->state = ENCODED_FAILED;
        }
        if(jp != NULL)
        {
            jp->clearParams();
        }
    }
    DEBUG_LEAVE();
}

void WebkitWrapper::stopLoadPage()
{
    DEBUG_ENTER();
    m_pPage->triggerAction(QWebPage::Stop);
    qDebug() << "WebkitWrapper::stopLoadPage() called";
    DEBUG_LEAVE();
}
