#ifndef _SERVER_WEBKIT_WRAPPER_WEBKITWRAPPER_H_
#define _SERVER_WEBKIT_WRAPPER_WEBKITWRAPPER_H_

#include <QtCore>
#include <QNetworkProxy>
#include <QThread>

#include "SWGlobal.h"
#include "SWWebPage.h"

class JobProcessor;

class WebkitWrapper : public QObject
{
	Q_OBJECT

public:
	WebkitWrapper(QNetworkProxy proxy, QObject *parent = 0);
	virtual ~WebkitWrapper();

	// Initialize in single-file mode.
	void open(const QUrl& url);

	SWWebPage *webPage() const { return m_pPage; };
	QhEventStruct* getRequest() const { return m_pRequest; };
	QhEventResponse *getResponse() const { return m_pResponse; };
	void handleRequest(QhEventStruct*, QhEventResponse *);
    void setPid(int input_pid) {pid = input_pid;};
    void setJP(JobProcessor *input_jp) {jp = input_jp;};
    void clearParams();
    SWWebPage *createWebPage();
    void stopLoadPage();

public slots:
    void updateMetadataStatus();
    void exceptUpdateMetadataStatus(bool ok);

Q_SIGNALS:
	void quit();

private:
	SWWebPage *m_pPage;

	QhEventStruct *m_pRequest;
	QhEventResponse *m_pResponse;
    JobProcessor *jp;
    int pid;
    QUrl url;
    QNetworkProxy pxy;
};

#endif // _SERVER_WEBKIT_WRAPPER_WEBKITWRAPPER_H__
