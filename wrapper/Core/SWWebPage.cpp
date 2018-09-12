#include "qwebhistory.h"
#include "SWWebPage.h"
#include "WebkitWrapper.h"
#include "SWLogger.h"


SWWebPage::SWWebPage(QObject* parent)
: QWebPage(parent)
{
}

SWWebPage::~SWWebPage()
{
    qDebug() << "SWWebPage's destruction function called";
}

void SWWebPage::clearHistory()
{
	// QWebHistory::clear() leaves current page, so remove it as well by setting
	// max item count to 0, and then setting it back to it's original value.
	QWebHistory* history = this->history();
	int itemCount = history->maximumItemCount();

	history->clear();
	history->setMaximumItemCount(0);
	history->setMaximumItemCount(itemCount);
}
