#ifndef _SERVER_WEBKIT_WRAPPER_GLOBAL_H__
#define _SERVER_WEBKIT_WRAPPER_GLOBAL_H__

#include <QMetaType>

#include "SWLogger.h"
#include "SWSettings.h"
#include "SWClientSocket.h"
#include <QhEventStruct.h>
#include "WebkitWrapper.h"

enum DataState
{
    NOT_BEGIN = 0,
    ACCEPTED,
    PROCESS,
    ENCODED_SUCCESS,
    ENCODED_FAILED,
    SENT
};

typedef struct {
    DataState state;
    SWClientSocket *socket;
    QhEventResponse *resp;
} metaData;

#endif // _SERVER_WEBKIT_WRAPPER_GLOBAL_H__
