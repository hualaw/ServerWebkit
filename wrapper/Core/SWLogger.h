#ifndef _SERVER_WEBKIT_WRAPPER_LOGGER_H__
#define _SERVER_WEBKIT_WRAPPER_LOGGER_H__

#include "Log4cplusWrapper.h"
#include "SWSettings.h"

class SWLogger: public QObject
{
    Q_OBJECT

public:
    SWLogger();
    ~SWLogger();
    //void log(const char *type, const char *message);
    void log(const char *type, QString message);
    Log4cplusWrapper *get_handle(std::string type);

private:
    Log4cplusWrapper *runLogWp;
    Log4cplusWrapper *errLogWp;
    Log4cplusWrapper *dbgLogWp;
    SWSettings settings;
};

#endif // _SERVER_WEBKIT_WRAPPER_LOGGER_H__
