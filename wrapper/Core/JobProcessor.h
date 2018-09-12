#ifndef _SERVER_WEBKIT_WRAPPER_JOBPROCESSOR_H_
#define _SERVER_WEBKIT_WRAPPER_JOBPROCESSOR_H_

#include <QTimer>
#include "SWGlobal.h"
#include "ProtocolAnalyzer.h"

class JobProcessor: public QObject
{
	Q_OBJECT

public:
	JobProcessor(QObject *parent = 0);
	~JobProcessor();
    void clearParams();

public slots:
    void start(int pid);
    void stopLoadPage();
    void stopTimer();

private:
    void startTimer(QObject *obj);

    int winNum;
	WebkitWrapper *webkit;
	ProtocolAnalyzer *pa;
    SWClientSocket *socket;
    QTimer *timer;
};

#endif //_SERVER_WEBKIT_WRAPPER_JOBPROCESSOR_H_
