#include "SWRequest.h"

SWCtrlEntry::SWCtrlEntry()
{
	id = 0;
}

SWCtrlEntry::SWCtrlEntry(SWCtrlEntry const &ctrl)
{
	id = ctrl.id;
	content = ctrl.getContent();
}

SWCtrlEntry& SWCtrlEntry::operator=(SWCtrlEntry const &ctrl)
{
	if(this == &ctrl)
		return *this;
	id = ctrl.id;
	content = ctrl.getContent();
	return *this;
}

SWCtrlEntry::~SWCtrlEntry()
{
	id = 0;
}

void SWCtrlEntry::setContent(QString str)
{
	content = str;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

SWRequest::SWRequest()
: flag(NONE)
{
	birthTime = QDateTime::currentMSecsSinceEpoch();

	isRedirect = 0;
	l = 0;
	ls = 0;
	pq = 0;
	isWap = 0;
	mx = 0;
	externalFilter = 0;  //init external filter
	templateFilter = 0;  
	loaded_size = 0;
	status = 0;

	seq = *(new QString());
	user_id = *(new QString());
	transaction_id = *(new QString());
	page_id = *(new QString());
	url = *(new QString());
	delete_cookies = *(new QString());

	status_text = *(new QString());
	content_type = *(new QString());
	x_jump = *(new QString());

	os = *(new QString());
}

SWRequest::SWRequest(SWRequest const &other)
: flag(other.flag)
{
	birthTime = QDateTime::currentMSecsSinceEpoch();

	type = other.type;
	tm = other.tm;
	screen_width = other.screen_width;
	screen_height = other.screen_height;
	zid = other.zid;
	font_width = other.font_width;

	xHeader.clear();
	for (int i = 0; i < other.xHeader.size(); i++)
	{
		xHeader.push_back(other.xHeader[i]);
	}

	wHeader.clear();
	for (int i = 0; i < other.wHeader.size(); i++)
	{
		wHeader.push_back(other.wHeader[i]);
	}

	lids.clear();
	for (int i = 0; i < other.lids.size(); i++)
	{
		lids.push_back(other.lids[i]);
	}

	zfont.clear();
	for (int i = 0; i < other.zfont.size(); i++)
	{
		zfont.push_back(other.zfont[i]);
	}

	controls.clear();
	for (int i = 0; i < other.controls.size(); i++) 
	{
		controls.push_back(other.controls[i]);
	}

	seq = other.getSeq();
	user_id = other.getUserID();
	transaction_id = other.getTransactionID();
	delete_cookies = other.getDeleteCookies();
	page_id = other.getPageID();
	url = other.getURL();

	isRedirect = other.isRedirect;
	l = other.l;
	ls = other.ls;
	pq = other.pq;
	isWap = other.isWap;
	mx = other.mx;
	externalFilter = other.externalFilter;
	templateFilter = other.templateFilter;
	loaded_size = other.loaded_size;
	status = other.status;
	status_text = other.getStatusText();
	content_type = other.getContentType();
	x_jump = other.getXJump();
	os = other.getOS();
}

SWRequest& SWRequest::operator=(SWRequest const &other)
{
	if(this == &other)
		return *this;

	birthTime = other.birthTime;
	type = other.type;
	tm = other.tm;
	screen_width = other.screen_width;
	screen_height = other.screen_height;
	zid = other.zid;
	font_width = other.font_width;

	xHeader.clear();
	for (int i = 0; i < other.xHeader.size(); i++)
	{
		xHeader.push_back(other.xHeader[i]);
	}

	wHeader.clear();
	for (int i = 0; i < other.wHeader.size(); i++)
	{
		wHeader.push_back(other.wHeader[i]);
	}

	lids.clear();
	for (int i = 0; i < other.lids.size(); i++)
	{
		lids.push_back(other.lids[i]);
	}

	zfont.clear();
	for (int i = 0; i < other.zfont.size(); i++)
	{
		zfont.push_back(other.zfont[i]);
	}

	controls.clear();
	for (int i = 0; i < other.controls.size(); i++) 
	{
		controls.push_back(other.controls[i]);
	}

	seq = other.getSeq();
	user_id = other.getUserID();
	transaction_id = other.getTransactionID();
	delete_cookies = other.getDeleteCookies();
	page_id = other.getPageID();
	url = other.getURL();

	isRedirect = other.isRedirect;
	l = other.l;
	ls = other.ls;
	pq = other.pq;
	isWap = other.isWap;
	mx = other.mx;
	externalFilter = other.externalFilter;
	templateFilter = other.templateFilter;
	loaded_size = other.loaded_size;
	status = other.status;
	status_text = other.getStatusText();
	content_type = other.getContentType();
	x_jump = other.getXJump();
	os = other.getOS();

	flag = other.flag;

	return *this;
}

SWRequest::~SWRequest()
{
	l = -1;
	mx = 0;
}

void SWRequest::setSeq(QString str)
{
	seq = str;
}

void SWRequest::setUserID(QString str)
{
	user_id = str;
}

void SWRequest::setURL(QString str)
{
	url = str;
}

void SWRequest::setTransactionID(QString str)
{
	transaction_id = str;
}

void SWRequest::setDeleteCookies(QString str)
{
	delete_cookies = str;
}

void SWRequest::setPageID(QString str)
{
	page_id = str;
}

void SWRequest::setStatusText(QString str)
{
	status_text = str;
}

void SWRequest::setContentType(QString str)
{
	content_type = str;
}

void SWRequest::setXJump(const QString str)
{
	x_jump = str;
}

void SWRequest::setOS(const QString str)
{
	os = str;
}

bool SWRequest::hasLid(unsigned int lid) const
{
	for (unsigned int i = 0; i < lids.size(); i++)
	{
		if (lid == lids[i])
			return true;
	}
	return false;
}

bool operator == (const SWRequest& eventSrc, const SWRequest& eventDest)
{
	if(&eventSrc == &eventDest)
	{
		return true;
	}
	else if(&eventSrc == NULL || &eventDest == NULL)
	{
		return false;
	}
	//If the same user send the OPEN_URL event and the url is the same
	//We can assume the events are identical 
	else if(eventSrc.type == OPEN_URL 
		&& eventDest.type == OPEN_URL
		&& eventSrc.getURL() == eventDest.getURL())
	{
		//printf("The user request to open the same url.\n");
		return true;    
	}
	else
	{
		return false;
	}
}



