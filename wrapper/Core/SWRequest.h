#ifndef _SERVER_WEBKIT_WRAPPER_REQUEST_H__
#define _SERVER_WEBKIT_WRAPPER_REQUEST_H__

#include <QtCore>

typedef QPair<QString, QString> QStringPair;

enum RequestFlag
{
	NONE             = 0,
	IGNORE_CURRENT   = 0x00000001,
	WML_EXECUTED     = 0x00000002,
	WILL_BE_RELOADED = 0x00000004,
	DUMPED           = 0x00000008
};

enum RequestType
{
	OPEN_URL = 0,
	COMMIT = 5
};

class SWCtrlEntry 
{
public:
	SWCtrlEntry();
	~SWCtrlEntry();

	SWCtrlEntry(const SWCtrlEntry &ctrl);
	SWCtrlEntry& operator=(const SWCtrlEntry &ctrl);
	const QString getContent() const { return content; }
	void setContent(const QString str, unsigned int length);
	void setContent(const QString str);

	int id;

private:
	QString content;
};

class SWRequest 
{
public:
	SWRequest();
	SWRequest(const SWRequest &other);
	~SWRequest();
	SWRequest & operator=(const SWRequest &other);
	friend bool operator == (const SWRequest& eventSrc, const SWRequest& eventDest);

	bool hasDumped()
	{
		return flag & DUMPED;
	}

	bool shouldBeByPassed()
	{
		return !(flag & WML_EXECUTED) && (flag & IGNORE_CURRENT) ;
	}

	uint getFlag() const {
		return flag;
	}

	void setFlag(uint newFlag) {
		flag = newFlag;
	}

	void addFlag(uint flagToAdd) {
		flag |= flagToAdd;
	}

	void removeFlag(uint flatToRemove) {
		flag &= ~flatToRemove;
	}

	const QString getSeq() const { return seq; }
	const QString getUserID() const { return user_id; }
	const QString getTransactionID() const { return transaction_id; }
	const QString getPageID() const { return page_id; }
	const QString getURL() const { return url; }
	const QString getDeleteCookies() const { return delete_cookies; }

	void setSeq(QString str);
	void setUserID(QString str);
	void setURL(QString str);

	void setTransactionID(QString str);
	void setPageID(QString str);
	void setDeleteCookies(QString str);

	int type;

	int tm;
	int screen_width;
	int screen_height;
	int font_width;
	int zid;

	int isRedirect;
	int l;
	int ls;
	int pq;
	int mx;
	int externalFilter;       //check external filter
	int templateFilter;      
	QVector<QStringPair> xHeader;
	QVector<QStringPair> wHeader;
	int isWap;
	int loaded_size;
	int status;
	const QString getStatusText() const { return status_text; }
	const QString getContentType() const { return content_type; }
	const QString getXJump() const { return x_jump; }
	const QString getOS() const { return os; }
	void setStatusText(QString str);
	void setContentType(QString str);
	void setXJump(QString str);
	void setOS(QString str);
	bool hasLid(unsigned int lid) const;

	QVector<int> lids;
	QVector<int> zfont;
	QVector< SWCtrlEntry > controls;

	int x;
	int y;

	qint64 birthTime;

private:
	QString seq;
	QString user_id;
	QString transaction_id;
	QString page_id;
	QString url;
	QString delete_cookies;

	QString status_text;
	QString content_type;
	QString x_jump;

	QString os;

	int flag;
};

#endif // _SERVER_WEBKIT_WRAPPER_REQUEST_H__
