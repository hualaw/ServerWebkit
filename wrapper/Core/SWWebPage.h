#ifndef _SERVER_WEBKIT_WRAPPER_WEB_PAGE_H__
#define _SERVER_WEBKIT_WRAPPER_WEB_PAGE_H__

#include "qwebpage.h"

class SWWebPage : public QWebPage
{
	Q_OBJECT

public slots:
    bool shouldInterruptJavaScript() {return true;};

public:
	SWWebPage(QObject* parent);
	virtual ~SWWebPage();
	void clearHistory();

};

#endif // _SERVER_WEBKIT_WRAPPER_WEB_PAGE_H__
