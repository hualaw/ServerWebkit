#include <QtCore>
//#include <QtGui>

#include "SWCookieJar.h"
#include "SWWebPage.h"

//For more log
//#define SWCOOKIE_VERBOSE

static const unsigned int JAR_VERSION = 23;
static const unsigned int buffer_size = 63*1024;

QT_BEGIN_NAMESPACE
QDataStream &operator<<(QDataStream &stream, const QList<QNetworkCookie> &list)
{
	unsigned int offset = 0;
	stream << JAR_VERSION;
	offset += sizeof(JAR_VERSION);
	stream << quint32(list.size());
	offset += sizeof(quint32);
	for (int i = 0; i < list.size(); ++i)	{
		QByteArray rawForm = list.at(i).toRawForm();
		//check the size of amount
		if( offset + rawForm.size() < buffer_size )	{
			stream << rawForm;
			offset += rawForm.size();
		}else	{
			//qDebug() <<  "The size of QList<QNetworkCookie>list is bigger than buffer size " << offset+rawForm.size();
			printf ( "%s:%d %s bigSize %u\n", __FILE__, __LINE__, __FUNCTION__, offset+rawForm.size() );
			break;
		}
	}	
	return stream;
}

QDataStream &operator>>(QDataStream &stream, QList<QNetworkCookie> &list)
{
	list.clear();

	quint32 version;
	stream >> version;

	if (version != JAR_VERSION)	{
		printf ( "%s:%d %s VERSION WRONG!!! %u\n", __FILE__, __LINE__, __FUNCTION__, version );
		//qDebug() << "VERSION WRONG!!! " << version;
		return stream;
	}

	quint32 count;
	stream >> count;
	for(quint32 i = 0; i < count; ++i)     {
		QByteArray value;
		stream >> value;
		QList<QNetworkCookie> newCookies = QNetworkCookie::parseCookies(value);
		if (newCookies.count() == 0 && value.length() != 0) {
			qWarning() << "CookieJar: Unable to parse saved cookie:" << value;
		}
		for (int j = 0; j < newCookies.count(); ++j)
			list.append(newCookies.at(j));
		if (stream.atEnd())
			break;
	}
	return stream;
}
QT_END_NAMESPACE

SWCookieJar::SWCookieJar( QString uid, QString tid, QObject *parent )
: QNetworkCookieJar( parent )
{
	m_uid = uid;
	m_tid = tid;
	m_domain = QString("0");
	m_page = static_cast<SWWebPage*>(parent);
}

SWCookieJar::~SWCookieJar( )
{
}

QList<QNetworkCookie> SWCookieJar::cookiesForUrl(const QUrl &url) const
{
	//qDebug() << url;
	return QNetworkCookieJar::cookiesForUrl(url);
}

bool SWCookieJar::setCookiesFromUrl( const QList<QNetworkCookie> &cookieList, const QUrl &url)
{
	QString host = url.host();
	bool addedCookies = false;

	// pass url domain == cookie domain
	QDateTime soon = QDateTime::currentDateTime();
	soon = soon.addDays(30);
	foreach(QNetworkCookie cookie, cookieList) {
		QList<QNetworkCookie> lst;
		if ( !cookie.isSessionCookie()
			&& cookie.expirationDate() > soon) {
				cookie.setExpirationDate(soon);
		}
		lst += cookie;
		if (QNetworkCookieJar::setCookiesFromUrl(lst, url)) {
			addedCookies = true;
		} else {
			// finally force it in if wanted
			QList<QNetworkCookie> cookies = allCookies();
			cookies += cookie;
			setAllCookies(cookies);
			addedCookies = true;
			//qWarning() << "setCookiesFromUrl failed" << url << cookieList.value(0).toRawForm();
		}
	}
	//m_saveTimer->changeOccurred();
	/*if( m_page )
		m_page->saveCookie();*/
	//printf ( "%s:%d %s saveCookie\n", __FILE__, __LINE__, __FUNCTION__ );
	return true;
}

void SWCookieJar::clearCookies()
{	
	QList<QNetworkCookie> empty;
	//...
	setAllCookies( empty );
}

bool SWCookieJar::bNeedChangeDomain( QString newDomain )
{
	//Just make the domain invalidate.
	return false;

#if 0
	if( m_domain==QString("0") )
		return true;

	if( newDomain.indexOf(m_domain) >= 0 )
		return false;

	return true;
#endif
}

void SWCookieJar::changeDomain( QString domain )
{
	clearCookies();
	setDomain( domain );
	load();
}

void SWCookieJar::changeUser( QString uid, QString tid )
{
	if( m_uid==uid && m_tid==tid )
		return;

	clearCookies();
	m_uid = uid;
	m_tid = tid;
	m_domain = QString("0");

	//Just load cookies when changeDomain is called
	//load();
}

int SWCookieJar::getExptime()
{
	QNetworkCookie ck = allCookies().at(0);
	QDateTime now = QDateTime::currentDateTime();

	//Maxisum is 1 day
	int ret = now.secsTo(ck.expirationDate());
	if( ret > 86400 )
		return 86400;
	else 
		return ret;

	//Default is 30mins
	//return 60*30;
}


void SWCookieJar::save()
{
	if( allCookies().size() == 0 )	
		return;

	QByteArray bai;
	QDataStream dsi(&bai, QIODevice::ReadWrite);
	dsi << allCookies();

	if( bNeedChangeDomain( allCookies().at(0).domain() ) )	{
		setDomain(  allCookies().at(0).domain() );
	}

#ifdef SWCOOKIE_VERBOSE
	printf ( "%s:%d %s %s:%s:%s\n", __FILE__, __LINE__, __FUNCTION__, m_uid.toUtf8().data(), m_tid.toUtf8().data(), m_domain.toUtf8().data() );
	printf ( "%s:%d %s allCookies size=%d\n", __FILE__, __LINE__, __FUNCTION__, allCookies().size() );
	for( int i=0; i<allCookies().size(); i++)
		printf ( "%s:%d %s [%d]:%s \n", __FILE__, __LINE__, __FUNCTION__, i, allCookies().at(i).toRawForm().data() );
#endif

	//Maybe do something to filter
	//...

	/*if( !swDbInf()->setData( m_uid, m_tid, m_domain, bai, getExptime() ) ){
#ifdef SWCOOKIE_VERBOSE
		printf ( "%s:%d %s setData failed\n", __FILE__, __LINE__, __FUNCTION__ );
#endif
	}*/

	fflush(stdout);
}

bool SWCookieJar::deleteDomains(QString domains)
{
	if( domains.size()==0 )
		return false;

#ifdef SWCOOKIE_VERBOSE
	printf ( "%s:%d %s %s:%s:%s\n", __FILE__, __LINE__, __FUNCTION__, m_uid.toUtf8().data(), m_tid.toUtf8().data(), m_domain.toUtf8().data() );
	printf ( "%s:%d %s allCookies size=%d\n", __FILE__, __LINE__, __FUNCTION__, allCookies().size() );
	for( int i=0; i<allCookies().size(); i++)
		printf ( "%s:%d %s [%d]:%s \n", __FILE__, __LINE__, __FUNCTION__, i, allCookies().at(i).toRawForm().data() );
#endif

	//parser domains
	QStringList sl = domains.split(QLatin1String(","));

	//check domains
	QList<QNetworkCookie> newCookies;
	for( int i=0; i<allCookies().size(); i++)	{
		int j=0;
		for( j=0; j<sl.size(); j++ )	{
			if( allCookies().at(i).domain().indexOf(sl[j])>=0 )	{
				break;
			}
		}
		//not match to save
		if( j==sl.size() )	{
			//newCookies.push_back(allCookies().at(i));
			newCookies.append(allCookies().at(i));
		}
	}

	//save cookie
	setAllCookies(newCookies);
	if( newCookies.size()>0 )
		save();
	/*else
		swDbInf()->delData(m_uid,m_tid,m_domain);*/

	return true;
}

void SWCookieJar::setDomain( QString domain )
{
	//if( domain.at(domain.size()-1).toAscii() >= '0' && domain.at(domain.size()-1).toAscii() <= '9' )	{
	//	m_domain = domain;
	//}else	{
	//	m_domain = domain.right( domain.size() - domain.indexOf(QChar('.')) );
	//}
}

void SWCookieJar::load()
{
	QList<QNetworkCookie> newCookies;
	QByteArray bao;

	/*if( swDbInf()->getData( m_uid, m_tid, m_domain, bao ) ){
		QDataStream dso(&bao, QIODevice::ReadWrite);
		dso >> newCookies;
		setAllCookies(newCookies);
	}else{
#ifdef SWCOOKIE_VERBOSE
		printf ( "%s:%d %s getData failed \n", __FILE__, __LINE__, __FUNCTION__ );
#endif
	}	*/

#ifdef SWCOOKIE_VERBOSE
	printf ( "%s:%d %s %s:%s:%s\n", __FILE__, __LINE__, __FUNCTION__, m_uid.toUtf8().data(), m_tid.toUtf8().data(), m_domain.toUtf8().data() );
	printf ( "%s:%d %s cookies size=%d\n", __FILE__, __LINE__, __FUNCTION__, newCookies.size() );
	for( int i=0; i<newCookies.size(); i++)	
		printf ( "%s:%d %s [%d]:%s \n", __FILE__, __LINE__, __FUNCTION__, i, newCookies.at(i).toRawForm().data() );
#endif

	fflush(stdout);
}

QString SWCookieJar::domain() const
{
	return m_domain;
}

QString SWCookieJar::tid() const
{
	return m_tid;
}

QString SWCookieJar::user() const
{
	return m_uid;
}
