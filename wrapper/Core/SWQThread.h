#ifndef _SERVER_WEBKIT_WRAPPER_SWQThREAD_H__
#define _SERVER_WEBKIT_WRAPPER_SWQThREAD_H__

#include <QThread>

class SWQThread : public QThread
{
    public:
        static void myUsleep(unsigned long usecs);
};

#endif //_SERVER_WEBKIT_WRAPPER_SWQThREAD_H__
