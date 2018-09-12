#ifndef _SERVER_WEBKIT_WRAPPER_COOKIEJAR_H_
#define _SERVER_WEBKIT_WRAPPER_COOKIEJAR_H_

#include <QtNetwork/QNetworkCookieJar>
#include <QtCore/QStringList>

class SWWebPage;

class SWCookieJar : public QNetworkCookieJar
{
	Q_OBJECT

public:
	SWCookieJar( QString uid, QString tid, QObject *parent );
	~SWCookieJar();

	QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const;
	bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);
	bool bNeedChangeDomain( QString newDomain );
	void changeDomain( QString domain );
	void changeUser( QString uid, QString tid );
	QString domain() const;
	QString user() const;
	QString tid() const;
	SWWebPage* page() const { return m_page;}
	void save();
	bool deleteDomains(QString domains);

private:
	void load();
	int getExptime();
	void clearCookies();
	void setDomain( QString domain );
	QString m_uid;
	QString m_tid;
	QString m_domain;
	SWWebPage *m_page;
};

#endif // _SERVER_WEBKIT_WRAPPER_COOKIEJAR_H_
